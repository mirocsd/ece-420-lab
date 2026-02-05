#include "common.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
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
pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;

char **theArray;
double *timeArray;
static void *thread_start(void *threadArg);
int serverfd;
int clientfd[COM_NUM_REQUEST];
int timeArrayIndex = 0;

int main(int argc, char **argv) {
  int num_positions;
  char server_ip[20];
  uint16_t server_port;
  pthread_t threads[COM_NUM_REQUEST];
  char initialMessage[30] = "String %d: the initial value";
  struct sockaddr_in sock_var;
  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  char current_line[COM_BUFF_SIZE];
  ClientRequest current_request;
  uint8_t created[COM_NUM_REQUEST] = {0};

  if (argc != 4) {
    printf("Error: Not enough input arguments");
    return 0;
  }

  num_positions = atoi(argv[1]);
  strncpy(server_ip, argv[2], 20);
  server_port = atoi(argv[3]);

  timeArray = malloc(COM_NUM_REQUEST * sizeof(double));

  theArray = (char**) malloc(num_positions * sizeof(char*));
  for (int i = 0; i < num_positions; i ++) 
  {
      theArray[i] = (char*) malloc(COM_BUFF_SIZE * sizeof(char));
      sprintf(theArray[i], initialMessage, i);
  }

  mutexes = malloc(num_positions * sizeof(pthread_mutex_t));

  for (int i = 0; i < num_positions; i++)
    pthread_mutex_init(&(mutexes[i]), NULL);

  sock_var.sin_addr.s_addr = inet_addr(server_ip);
  sock_var.sin_port=(int)server_port;
  sock_var.sin_family=AF_INET;
  if (bind(serverfd, (struct sockaddr*)&sock_var, sizeof(sock_var)) >= 0)
  {
    listen(serverfd, 2000);
    while (1)
    {
      for (int i = 0; i < COM_NUM_REQUEST; i++) created[i] = 0;

      for (int i = 0; i < COM_NUM_REQUEST; i++)
      {
        clientfd[i] = accept(serverfd, NULL, NULL);
        if (clientfd[i] < 0) { created[i] = 0; continue; }

        ssize_t size = read(clientfd[i], current_line, COM_BUFF_SIZE);
        if (size <= 0) {
          close(clientfd[i]);
          created[i] = 0;
          continue;
        }
        current_line[size] = '\0';
        printf("current line %s\n", current_line);
        if (strchr(current_line, '-') != NULL) {
          if (strchr(current_line, '-') != NULL) {
            ParseMsg(current_line, &current_request);
          }
          else {
            close(clientfd[i]);
            created[i] = 0;
            continue;
          }
        }
        else {
          close(clientfd[i]);
          created[i] = 0;
          continue;
        }
        threadArgs *arg = (threadArgs*)malloc(sizeof(threadArgs));
        if (arg == NULL) {
          close(clientfd[i]);
          created[i] = 0;
          continue;
        }
        arg->current_request = current_request;
        arg->clientfd = clientfd[i];
        int status = pthread_create(&threads[i], NULL, thread_start, (void*)arg);
        if (status != 0) {
          free(arg);
          close(clientfd[i]);
          created[i] = 0;
          continue;
        }

        created[i] = 1;

        if (timeArrayIndex >= COM_NUM_REQUEST) {
          saveTimes(timeArray, COM_NUM_REQUEST);
          timeArrayIndex = 0;
        }
      }

      for (int i = 0; i < COM_NUM_REQUEST; i++) {
        if (created[i]) pthread_join(threads[i], NULL);
      } 
    }
  }
  else
  {
    printf("Error: Failed to bind socket");
    return 0;
  }


}

static void* thread_start(void *threadArg)
{
  threadArgs *requestArgs = (threadArgs*)threadArg;
  if (requestArgs->clientfd < 0) {
    free(requestArgs);
    return NULL;
  }
  char result[COM_BUFF_SIZE];
  double start_time, end_time, total_time;
  GET_TIME(start_time);
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
    getContent(result, requestArgs->current_request.pos, theArray);
    write(requestArgs->clientfd, result, COM_BUFF_SIZE);
  }
  GET_TIME(end_time);
  total_time = end_time - start_time;
  pthread_mutex_lock(&time_mutex);
  timeArray[timeArrayIndex] = total_time;
  timeArrayIndex++;
  pthread_mutex_unlock(&time_mutex);
  pthread_mutex_unlock(&mutexes[requestArgs->current_request.pos]);
  shutdown(requestArgs->clientfd, SHUT_RDWR);
  close(requestArgs->clientfd);
  free(requestArgs);

  return NULL;
}
