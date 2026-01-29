#include "common.h"
#include "server.h"
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>


pthread_mutex_t *mutexes;
pthread_mutex_t work_mutex;
pthread_cond_t work_ready;
struct Work *workHead;

static void *thread_start(void *threadArg);

int main(int argc, char **argv) {
  int num_positions;
  char server_ip[20];
  uint8_t server_port;
  pthread_t threads[COM_NUM_REQUEST];
  struct Work *workTail;
  char *initialMessage = "String %d: the initial value";
  char **theArray;

  if (argc != 4) {
    printf("Error: Not enough input arguments");
    return 0;
  }

  num_positions = atoi(argv[1]);
  strcpy(argv[2], server_ip);
  server_port = atoi(argv[3]);

  theArray = (char**) malloc(num_positions * sizeof(char*));
  for (int i = 0; i < num_positions; i ++) 
  {
      theArray[i] = (char*) malloc(strlen(initialMessage) * sizeof(char) + 3);
      sprintf(theArray[i], initialMessage, i);
  }


  workHead = malloc(sizeof(struct Work));

  pthread_mutex_init(&work_mutex, NULL);

  pthread_cond_init(&work_ready, NULL);

  mutexes = malloc(num_positions * sizeof(pthread_mutex_t));

  for (int i = 0; i < COM_NUM_REQUEST; i++)
    pthread_mutex_init(&(mutexes[i]), NULL);

  for (int i = 0; i < COM_NUM_REQUEST; i++)
  {
    pthread_create(&threads[i], NULL, thread_start, (void*)NULL);
  }

  /* process server requests, update linked list, notify threads that work is ready */

}

static void* thread_start(void *threadArg)
{
  (void)threadArg;

  pthread_mutex_lock(&work_mutex);
  pthread_cond_wait(&work_ready, &work_mutex);

  struct Work *thisWork;
  thisWork = workHead;
  return NULL;
}
