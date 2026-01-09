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

int main(int argc, char* argv[]) {
    (void)argc;
    long thread;
    double total_time;
    pthread_t* thread_handles;
    double start_time, end_time;
    blockLocation *blocks;
    double root_p;
    
    /* set p to the number of threads, argv[1] */
    p = atoi(argv[1]);
  
    thread_handles = (pthread_t *)malloc(p*sizeof(pthread_t));

    Lab1_loadinput(&A, &B, &n);

    /* initialize matrix C */
    C = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        C[i] = (int *)malloc(n * sizeof(int));
    
    /* 
     * initialize array storing pointers to blocks that will
     * need to be freed later
     */
    blocks = (blockLocation *)malloc(sizeof(*blocks) * p);
    
    ThreadArg *args = malloc(p * sizeof(*args));
    // start time
    GET_TIME(start_time);
    
    /* compute sqrt(p) once */
    root_p = sqrt(p);

    for (thread = 0; thread < p; thread++) {
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
        
    // end time
    GET_TIME(end_time);

    total_time = end_time - start_time;

    Lab1_saveoutput(C, &n, total_time);
    
    free(blocks);

    free(args);  

    for (int i = 0; i < n; i++)
    {
      free(A[i]);
      free(B[i]);
      free(C[i]);
    }

    free(A);
    free(B);
    free(C);
    
    free(thread_handles);

    return 0;
}
