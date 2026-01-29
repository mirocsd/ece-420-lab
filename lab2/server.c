#include "common.h"
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>


pthread_mutex_t *mutexes;
pthread_mutex_t work_mutex;
pthread_cond_t work_ready;

static void *thread_start(void *threadArg);

int main(int argc, char **argv) {
  int num_positions;
  char server_ip[20];
  uint8_t server_port;
  pthread_t threads[COM_NUM_REQUEST];

  if (argc != 4)
    printf("Error: Not enough input arguments");

  num_positions = atoi(argv[1]);
  strcpy(argv[2], server_ip);
  server_port = atoi(argv[3]);

  pthread_mutex_init(&work_mutex, NULL);

  pthread_cond_init(&work_ready, NULL);

  mutexes = malloc(num_positions * sizeof(pthread_mutex_t));

  for (int i = 0; i < COM_NUM_REQUEST; i++)
    pthread_mutex_init(&(mutexes[i]), NULL);

  for (int i = 0; i < COM_NUM_REQUEST; i++)
  {
    pthread_create(&threads[i], NULL, thread_start, (void*)NULL);
  }

}

static void* thread_start(void *threadArg)
{
  (void)threadArg;





  return NULL;
}
