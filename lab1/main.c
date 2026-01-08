#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "lab1_IO.h"
#include "main.h"

int main(int argc, char* argv[]) {
    int p = argv[1]; //argv[1] is the number of threads
    long thread;
    pthread_t* thread_handles;
    thread_handles = malloc(p*sizeof(pthread_t));
    int **A, **B, n;
    Lab1_loadinput(int &A, int &B, int &n);

    // Creating Matrix C
    int **C = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
    }

    for (thread = 0; thread < p; thread++)
        pthread_create(&thread_handles[thread], NULL,
            Pth_mat_mat, (void*) thread);
    for (thread = 0; thread < p; thread++)
        pthread_join(thread_handles[thread], NULL);

    Lab1_saveoutput(&C, n, double Time)

    int x = floor(k/sqrt(p));
    int y = k%sqrt(p);
    int first_row = (n/sqrt(p))*x;
    int last_row = (n/sqrt(p))*(x+1);
    int first_column = (n/sqrt(p))*y;
    int last_column = (n/sqrt(p))*(y+1);

    free(A);
    free(B);
    free(C);

    return 0;
}

/*------------------------------------------------------------------
* Function: Pth_mat_mat
* Purpose: Multiply an nxn matrix by an nxn matrix
* In arg: rank
* Global in vars: A, B, n //where n is the size
* Global out var: C
*/
void *Pth_mat_mat(void* rank) {
	long my_rank = (long) rank;
	int row = my_rank/n;
	int column = my_rank%n;
	C[row][column] = 0;
	for (int i = 0; i < n; i++) {
		C[row][column] += A[row][i]*B[i][column];
	}
	return NULL;
}

/* rank is the rank of this thread, or k in the formula given */
void *pthr_matbymat(void *rank)
{
  blockLocation thisBlock;
  long my_rank = (long) rank;
  thisBlock.row = math.floor(my_rank/math.sqrt(p));
  thisBlock.col = my_rank % math.sqrt(p);

}
