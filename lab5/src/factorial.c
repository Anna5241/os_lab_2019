#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Структура для передачи данных в поток
typedef struct {
    int start;
    int end;
    int mod;
    int base_mod;
    long long *result;
    pthread_mutex_t *lock;
} ThreadData;

// Функция для вычисления частичного факториала
void *partial_factorial(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long long partial_result = 1;
    for (int i = data->start; i < data->end; ++i) {
        if (i % data->mod == data->base_mod) {
            partial_result = (partial_result * i) % data->mod;
        }
    }
    pthread_mutex_lock(data->lock);
    *data->result = (*data->result * partial_result) % data->mod;
    pthread_mutex_unlock(data->lock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<mod>\n", argv[0]);
        return 1;
    }

    int k = atoi(argv[2]);
    int pnum = atoi(argv[4]);
    int mod = atoi(argv[6]);

    if (k < 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Invalid input parameters\n");
        return 1;
    }

    // Инициализация результата и мьютекса
    long long result = 1;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    // Создание потоков
    pthread_t threads[pnum];
    ThreadData data[pnum];
    int chunk_size = k / pnum;
    int base_mod = k % mod;

    for (int i = 0; i < pnum; ++i) {
        data[i].start = i * chunk_size + 1;
        data[i].end = (i == pnum - 1) ? k + 1 : (i + 1) * chunk_size + 1;
        data[i].mod = mod;
        data[i].base_mod = base_mod;
        data[i].result = &result;
        data[i].lock = &lock;
        pthread_create(&threads[i], NULL, partial_factorial, (void *)&data[i]);
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Освобождение мьютекса
    pthread_mutex_destroy(&lock);

    // Вывод результата
    printf("Factorial of %d modulo %d is: %lld\n", k, mod, result);

    return 0;
}
