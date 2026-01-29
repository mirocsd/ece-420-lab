#include "common.h"
#include "server.h"
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>


pthread_mutex_t *mutexes;
#define MESSAGE_LEN = 100;
static void *thread_start(void *threadArg);

int main(int argc, char **argv) {
  int num_positions;
  char server_ip[20];
  uint16_t server_port;
  pthread_t threads[COM_NUM_REQUEST];
  char *initialMessage = "String %d: the initial value";
  char **theArray;
  struct sockaddr_in sock_var;
  int serverfd = socket(AF_INET, SOCK_STREAM, 0);
  int clientfd;
  struct Work current_work;
  char current_line[MESSAGE_LEN];

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

  mutexes = malloc(num_positions * sizeof(pthread_mutex_t));

  for (int i = 0; i < COM_NUM_REQUEST; i++)
    pthread_mutex_init(&(mutexes[i]), NULL);

  /* process server requests, update linked list, notify threads that work is ready */
  sock_var.sin_addr.s_addr = inet_addr(server_ip);
  sock_var.sin_port=htons((int)server_port);
  sock_var.sin_family=AF_INET;

  if (bind(serverfd, (struct sockaddr*)&sock_var, sizeof(sock_var)) >= 0)
  {
    while (1)
    {
      for (int i = 0; i < COM_NUM_REQUEST; i++)
      {
        clientfd = accept(serverfd, NULL, NULL);
        current_work.fd = clientfd;

        current_line = read(clientfd, current_line, MESSAGE_LEN);

        pthread_create(&threads[i], NULL, thread_start, (void*)&current_work);
      }
    }



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
