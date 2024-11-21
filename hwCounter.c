#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long globalCounter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
  int thresh;
  long goal;
} CounterArgs;

void *counter(void *arg) {
  CounterArgs *args = (CounterArgs *)arg;
  for (int i = 1; i <= args->goal; i++) {
    if (i % args->thresh == 0) {
      pthread_mutex_lock(&mutex);
      globalCounter += args->thresh;
      pthread_mutex_unlock(&mutex);
    }
  }
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <thresh>\n", argv[0]);
    return 1;
  }

  int thresh = atoi(argv[1]);

  pthread_t threads[5];
  CounterArgs *args[5];
  struct timespec start, end;
  double elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < 5; i++) {
    args[i] = malloc(sizeof(CounterArgs));
    args[i]->goal = 1000000000;
    args[i]->thresh = thresh;

    pthread_create(&threads[i], NULL, counter, (void *)args[i]);
  }

  for (int i = 0; i < 5; i++) {
    pthread_join(threads[i], NULL);

    free(args[i]);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  // Calculate elapsed time in milliseconds
  elapsed = (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1000000000.0;

  printf("Threshold: %d\tElapsed time: %.3f seconds\n", thresh, elapsed);

  return 0;
}
