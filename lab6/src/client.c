#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <utils.h>

struct Server {
  char ip[255];
  int port;
};


bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255
  // TODONE: because of 255 char restriction in linux file naming convention
  
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        if (!ConvertStringToUI64(optarg, &k))
          return 1;
        
        break;
      case 1:
        if (!ConvertStringToUI64(optarg, &mod))
          return 1;

        break;
      case 2:
        if (strlen(optarg) > 255)
        {
          fprintf(stderr, "Filename is too long\n %s\n", optarg);
          return 1;
        }
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  FILE *hostname_file = fopen(servers, "r"); // читаем файл со списком серверов
  if (!hostname_file)
  {
    fprintf(stderr, "Cannot open file: %s", servers);
    return 1;
  }
  unsigned int lines = 0;

  int32_t last = '\n';
  int32_t c;
  while (EOF != (c = fgetc(hostname_file))) { // считаем количество записей в файле с учётом пустых строк
    if (c == '\n' && last != '\n') {
      ++lines;
    }
    last = c;
  }
  unsigned int servers_num = lines + 1;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  if (to == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    fclose(hostname_file);
    return 1;
  }
  
  rewind(hostname_file); // начинаем читать с начала 
  int j = 0;

  while (fscanf(hostname_file, "%254[^:]:%d\n", to[j].ip, &to[j].port) == 2) // парсим файл в массив to
    if (j++ >= servers_num)
      break;

  fclose(hostname_file);

  uint64_t final_answer = 1;

  for (int i = 0; i < servers_num; i++) {
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL) { // проверка валидности ip
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);
    }

    // подготовка к инициализации сокета клиента
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

    // открываем сокет
    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    // подключаемся к серверу
    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    // распределяем нагрузку
    uint64_t range = k / servers_num;
    uint64_t begin = i * range;
    uint64_t end = (i == servers_num - 1) ? k : (i + 1) * range;

    // формируем сообщение серверу
    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    // отправляем сообщение
    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    // ожидаем ответа (читаем по приходу сообщений)
    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Recieve failed\n");
      exit(1);
    }

    // обрабатываем ответ и формируем результат
    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));

    final_answer = MultModulo(final_answer, answer, mod);
    close(sck);
  }
  printf("final answer: %lu\n", final_answer % mod);
  free(to);

  return 0;
}
