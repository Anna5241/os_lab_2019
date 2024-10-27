#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Мьютексы для демонстрации deadlock
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

// Функция для первого потока
void *thread1(void *arg) {
    printf("Thread 1: trying to lock mutex1\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 1: locked mutex1\n");

    sleep(1); // Задержка для демонстрации deadlock

    printf("Thread 1: trying to lock mutex2\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 1: locked mutex2\n");

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    return NULL;
}

// Функция для второго потока
void *thread2(void *arg) {
    printf("Thread 2: trying to lock mutex2\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 2: locked mutex2\n");

    sleep(1); // Задержка для демонстрации deadlock

    printf("Thread 2: trying to lock mutex1\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 2: locked mutex1\n");

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Инициализация мьютексов
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    // Создание потоков
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);

    // Ожидание завершения потоков
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Освобождение мьютексов
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return 0;
}
