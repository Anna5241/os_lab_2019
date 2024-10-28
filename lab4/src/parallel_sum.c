#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sum_lib.h"
#include "../../lab3/src/utils.h"

#include <pthread.h>

void GenerateArray(int *array, unsigned int array_size, unsigned int seed);

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {

  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  pthread_t threads[threads_num];

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--threads_num") == 0) {
      threads_num = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--seed") == 0) {
      seed = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--array_size") == 0) {
      array_size = atoi(argv[++i]);
    }
  }

  int *array = malloc(sizeof(int) * array_size);
  if (array == NULL) {
    printf("Error: malloc failed!\n");
    return 1;
  }

  if (threads_num == 0 || array_size == 0) {
    printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
    return 1;
  }

  GenerateArray(array, array_size, seed);

  struct SumArgs args[threads_num];
  
  int chunk_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size;
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      free(array);
      return 1;
    }
  }

  clock_t start = clock();

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  clock_t end = clock();
  double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

  free(array);
  printf("Total: %d\n", total_sum);
  printf("Time: %f seconds\n", time_spent);
  return 0;
}
