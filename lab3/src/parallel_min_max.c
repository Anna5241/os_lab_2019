#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("seed is a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("array size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
              printf("thread number is a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int(*pipe_fd)[2];
  if(!with_files)
    pipe_fd = malloc(pnum * sizeof(int[2]));

  pid_t child_pid;

  if(with_files)
    fclose(fopen(".shared_data.txt", "w"));

  for (int i = 0; i < pnum; i++) {

    if(!with_files && pipe(pipe_fd[i]) == -1){
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    child_pid = fork();

    if (child_pid >= 0) {
      struct MinMax forked_min_max;
      active_child_processes += 1;
      if (child_pid == 0) {
        unsigned begin_ = i * (array_size / pnum);
        unsigned end_ = begin_ + array_size / pnum;
        if((i + 1) >= pnum)
          end_ = array_size;
        forked_min_max =  GetMinMax(array, begin_, end_);

        if (with_files) {
          FILE *fd = fopen(".shared_data.txt", "a+");
          fclose(fd);
        } else {
          close(pipe_fd[i][0]);
          write(pipe_fd[i][1], &forked_min_max, sizeof(forked_min_max));
          close(pipe_fd[i][1]);
        }
        _exit(EXIT_SUCCESS);
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    active_child_processes -= 1;
    wait(NULL);
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  FILE *fd;
  if(with_files)
    fd = fopen(".shared_data.txt", "r");

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      fscanf(fd, "%d %d\n", &min, &max);
    } else {
      
      struct MinMax piped_min_max;

      if(read(pipe_fd[i][0], &piped_min_max, sizeof(piped_min_max)) == -1)
        printf("bad pipe\n");

      close(pipe_fd[i][0]);
      close(pipe_fd[i][1]);

      min = piped_min_max.min;
      max = piped_min_max.max;
    }
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  if(with_files)
    fclose(fd);

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  if(!with_files)
    free(pipe_fd);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
