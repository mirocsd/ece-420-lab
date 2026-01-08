#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "lab1_IO.h"
#include "main.h"
#include "timer.h"
#include "multiply.h"

extern int **A, **B, **C, n;
extern int p;

int main(int argc, char* argv[]) {
    p = argv[1]; //argv[1] is the number of threads
    long thread;
    pthread_t* thread_handles;
    double start_time, end_time;
    thread_handles = malloc(p*sizeof(pthread_t));
    Lab1_loadinput(&A, &B, &n);

    // Creating Matrix C
    **C = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
    }
    
    //start time
    GET_TIME(start_time);
    double start, end;
    for (thread = 0; thread < p; thread++) {
        int x = floor(thread/sqrt(p));
        int y = thread % (int) sqrt(p);
        int first_row = (n/sqrt(p))*x;
        int last_row = (n/sqrt(p))*(x+1);
        int first_column = (n/sqrt(p))*y;
        int last_column = (n/sqrt(p))*(y+1);
        pthread_create(&thread_handles[thread], NULL, Pth_mat_mat(), (void*) thread);
    }
        
    for (thread = 0; thread < p; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
        
    //end time
    GET_TIME(end_time);

    double Time = end_time - start_time;

    Lab1_saveoutput(&C, n, Time);

    free(A);
    free(B);
    free(C);

    return 0;
}
