#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab3IO.h"

int main() {
    double **G;
    int n;
    Lab3LoadInput(&G, &n);

    FILE *fp = fopen("data_output", "r");
    if (!fp)
        return 1;
    

    int out_n;
    fscanf(fp, "%d\n", &out_n);
    if (out_n != n) {
        fclose(fp);
        return 1;
    }

    double *x = CreateVec(n);
    for (int i = 0; i < n; i++)
        fscanf(fp, "%le", &x[i]);
    fclose(fp);

    double max_err = 0.0;
    for (int i = 0; i < n; i++) {
        double ax_i = 0.0;
        for (int j = 0; j < n; j++)
            ax_i += G[i][j] * x[j];
        double err = fabs(ax_i - G[i][n]);
        if (err > max_err)
            max_err = err;
    }

    printf("Max error = %e\n", max_err);

    DestroyMat(G, n);
    DestroyVec(x);
    return 0;
}