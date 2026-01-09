#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "lab1_IO.h"
#include "main.h"
#include "timer.h"
#include "multiply.h"

int **A, **B, **C, n;
long p;

static void init_globals(char **argv);
static void free_globals();

int main(int argc, char* argv[])
{
  long thread;
  double root_p;
  double total_time, start_time, end_time;

  pthread_t* thread_handles;
  blockLocation *blocks;
  ThreadArg *args;
  
  (void)argc;
  Lab1_loadinput(&A, &B, &n);
  init_globals(argv);

  thread_handles = malloc(p * sizeof(pthread_t));
  blocks = malloc(p * sizeof(*blocks));
  args = malloc(p * sizeof(*args));

  /* start time */
  GET_TIME(start_time);
  
  /* compute sqrt(p) once! */
  root_p = sqrt(p);

  for (thread = 0; thread < p; thread++)
  {
    args[thread].thisBlock = &blocks[thread];
    args[thread].thisBlock->row = floor(thread/root_p);
    args[thread].thisBlock->col = thread % (int)root_p;
    args[thread].thisBlock->minRow = ((int)(n/root_p))*(args[thread].thisBlock->row);
    args[thread].thisBlock->maxRow = ((int)(n/root_p))*(args[thread].thisBlock->row+1);
    args[thread].thisBlock->minCol = ((int)(n/root_p))*(args[thread].thisBlock->col);
    args[thread].thisBlock->maxCol = ((int)(n/root_p))*(args[thread].thisBlock->col+1);
    pthread_create(&thread_handles[thread], NULL, thr_matbymat, (void*)&args[thread]);
  }
      
  for (thread = 0; thread < p; thread++)
    pthread_join(thread_handles[thread], NULL);
      
  /* end time */
  GET_TIME(end_time);

  total_time = end_time - start_time;
  Lab1_saveoutput(C, &n, total_time);
  
  free(blocks);
  free(args);  
  free(thread_handles);

  free_globals();  
  return 0;
}

static void init_globals(char **argv)
{
  /* set p to the number of threads, argv[1] */
  p = atoi(argv[1]);
  
  /* init matrix C */
  C = (int **)malloc(n * sizeof(int *));
  for (int i = 0; i < n; i++)
      C[i] = (int *)calloc(n, sizeof(**C));

  return;
}



static void free_globals()
{
  for (int i = 0; i < n; i++)
  {
    free(A[i]);
    free(B[i]);
    free(C[i]);
  }

  free(A);
  free(B);
  free(C);

  return;
}
