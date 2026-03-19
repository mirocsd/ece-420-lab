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

static const double epsilon_sq = EPSILON * EPSILON;

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
    double local_sums[2], global_sums[2];

    double base_rank = (1.0 - DAMPING_FACTOR) / nodecount;
    double *inv_out = malloc(nodecount * sizeof(double));
    for (i = 0; i < nodecount; ++i)
        inv_out[i] = 1.0 / nodehead[i].num_out_links;
    
    GET_TIME(start);
    // core calculation
    do {
        iterationcount++;
        for (i = 0; i < nodecount; ++i) r_pre[i] = r[i];
        for (i = local_start; i < local_end; ++i) {
            struct node *ni = &nodehead[i];
            int num_in = ni->num_in_links;
            int *inlinks = ni->inlinks;
            double sum = 0.0;
            for (j = 0; j < num_in; ++j) {
                int src = inlinks[j];
                sum += r_pre[src] * inv_out[src];
            }
            r[i] = DAMPING_FACTOR * sum + base_rank;
        }

        MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                       r, counts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

        local_sums[0] = 0.0; local_sums[1] = 0.0;
        for (i = local_start; i < local_end; ++i) {
            double d = r[i] - r_pre[i];
            local_sums[0] += d * d;
            local_sums[1] += r_pre[i] * r_pre[i];
        }
        MPI_Allreduce(local_sums, global_sums, 2, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    } while(global_sums[0] / global_sums[1] >= epsilon_sq);
    GET_TIME(end);

    free(counts); free(displs); free(inv_out);

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