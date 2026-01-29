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

typedef struct {
  ClientRequest current_request;
  int clientfd;
} threadArgs;


pthread_mutex_t *mutexes;
char **theArray;
static void *thread_start(void *threadArg);
int serverfd;
int clientfd[COM_NUM_REQUEST];

int main(int argc, char **argv) {
  int num_positions;
  char server_ip[20];
  uint16_t server_port;
  pthread_t threads[COM_NUM_REQUEST];
  char initialMessage[30] = "String %d: the initial value";
  struct sockaddr_in sock_var;
  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  struct Work current_work;
  char current_line[COM_BUFF_SIZE];
  ClientRequest current_request;

  if (argc != 4) {
    printf("Error: Not enough input arguments");
    return 0;
  }

  num_positions = atoi(argv[1]);
  strncpy(server_ip, argv[2], 20);
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
  sock_var.sin_port=(int)server_port;
  sock_var.sin_family=AF_INET;

  if (bind(serverfd, (struct sockaddr*)&sock_var, sizeof(sock_var)) >= 0)
  {
    listen(serverfd, 2000);
    while (1)
    {
      for (int i = 0; i < COM_NUM_REQUEST; i++)
      {
        clientfd[i] = accept(serverfd, NULL, NULL);
        read(clientfd[i], current_line, COM_BUFF_SIZE);
        ParseMsg(current_line, &current_request);
        threadArgs thisArg;
        thisArg.current_request = current_request;
        thisArg.clientfd = clientfd[i];

        pthread_create(&threads[i], NULL, thread_start, (void*)&thisArg);
      }
    }
  }


}

static void* thread_start(void *threadArg)
{
  threadArgs *requestArgs = (threadArgs*)threadArg;
  char result[COM_BUFF_SIZE];
  
  if (requestArgs->current_request.is_read == 1)
  {
    pthread_mutex_lock(&mutexes[requestArgs->current_request.pos]);
    getContent(result, requestArgs->current_request.pos, theArray);
    write(requestArgs->clientfd, result, COM_BUFF_SIZE);
  }
  else
  {
    pthread_mutex_lock(&mutexes[requestArgs->current_request.pos]);
    setContent(requestArgs->current_request.msg, requestArgs->current_request.pos, theArray);
  }

  pthread_mutex_unlock(&mutexes[requestArgs->current_request.pos]);

  return NULL;
}
