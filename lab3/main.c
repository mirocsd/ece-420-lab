#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "Lab3IO.h"
#include "timer.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);

    double **A;
    int n;
    Lab3LoadInput(&A, &n);

    double *x = CreateVec(n);
    double start, end;

    GET_TIME(start);
    #pragma omp parallel
    {
        for (int k = 0; k < n - 1; k++) {

            #pragma omp single
            {
                int kp = k;
                double max_val = fabs(A[k][k]);
                for (int i = k + 1; i < n; i++) {
                    if (fabs(A[i][k]) > max_val) {
                        max_val = fabs(A[i][k]);
                        kp = i;
                    }
                }

                if (kp != k) {
                    double *tmp = A[k];
                    A[k] = A[kp];
                    A[kp] = tmp;
                }
            }


            #pragma omp for schedule(dynamic)
            for (int i = k + 1; i < n; i++) {
                double temp = A[i][k] / A[k][k];
                for (int j = k; j <= n; j++) {
                    A[i][j] -= temp * A[k][j];
                }
            }
            
        }

        for (int k = n - 1; k >= 1; k--) {
            #pragma omp for schedule(dynamic)
            for (int i = 0; i < k; i++) {
                double factor = A[i][k] / A[k][k];
                A[i][n] -= factor * A[k][n];
                A[i][k] = 0.0;
            }
        }

        #pragma omp for
        for (int i = 0; i < n; i++) {
            x[i] = A[i][n] / A[i][i];
        }
    }
    GET_TIME(end);

    Lab3SaveOutput(x, n, end - start);

    DestroyMat(A, n);
    DestroyVec(x);
    return 0;
}