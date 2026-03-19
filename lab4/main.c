/*
    Serial Implementation of Lab 4
*/

#define LAB4_EXTEND

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

int main (int argc, char* argv[]){
    // instantiate variables
    struct node *nodehead;
    int nodecount;
    double *r, *r_pre;
    int i, j;
    int iterationcount;
    double start, end;
    FILE *ip;
    int numtasks, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // load data 
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return 253;
    }
    fscanf(ip, "%d\n", &nodecount);
    fclose(ip);
    if (node_init(&nodehead, 0, nodecount)) return 254;
    
    // initialize variables
    r = malloc(nodecount * sizeof(double));
    r_pre = malloc(nodecount * sizeof(double));

    iterationcount = 0;
    for (i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    /* INITIALIZE MORE VARIABLES IF NECESSARY */
    
    int local_start = rank * (nodecount / numtasks) + (rank < nodecount % numtasks ? rank : nodecount % numtasks);
    int local_end   = local_start + nodecount / numtasks + (rank < nodecount % numtasks ? 1 : 0);

    int *counts = malloc(numtasks * sizeof(int));
    int *displs = malloc(numtasks * sizeof(int));
    for (i = 0; i < numtasks; ++i) {
        counts[i] = nodecount / numtasks + (i < nodecount % numtasks ? 1 : 0);
        displs[i] = i * (nodecount / numtasks) + (i < nodecount % numtasks ? i : nodecount % numtasks);
    }
    double *r_local = malloc((local_end - local_start) * sizeof(double));

    double local_diff2, local_norm2, gdiff2, gnorm2;

    GET_TIME(start);
    // core calculation
    do {
        ++iterationcount;
        for (i = local_start; i < local_end; ++i) r_pre[i] = r[i];
        for (i = local_start; i < local_end; ++i) r[i] = 0.0;
        for (i = local_start; i < local_end; ++i){
            for (j = 0; j < nodehead[i].num_in_links; ++j){
                r[i] += r_pre[nodehead[i].inlinks[j]] / nodehead[nodehead[i].inlinks[j]].num_out_links;
            }
            r[i] = DAMPING_FACTOR * r[i] + (1 - DAMPING_FACTOR) / nodecount;
        }

        for (i = local_start; i < local_end; ++i) r_local[i - local_start] = r[i];
        MPI_Allgatherv(r_local, local_end - local_start, MPI_DOUBLE,
                       r, counts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

        local_diff2 = 0.0; local_norm2 = 0.0;
        for (i = local_start; i < local_end; ++i) {
            double d = r[i] - r_pre[i];
            local_diff2 += d * d;
            local_norm2 += r_pre[i] * r_pre[i];
        }
        MPI_Allreduce(&local_diff2, &gdiff2, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&local_norm2, &gnorm2, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    } while(sqrt(gdiff2 / gnorm2) >= EPSILON);
    GET_TIME(end);

    free(counts); free(displs); free(r_local);

    if (rank == 0) {
        Lab4_saveoutput(r, nodecount, end - start);
        printf("Time: %f\n", end - start);
    }

    // post processing
    node_destroy(nodehead, nodecount);
    free(r); 
    free(r_pre);
    MPI_Finalize();
    return 0;
}