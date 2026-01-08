#include <math.h>
#include "main.h"
#include "multiply.h"

extern long p;
extern int **A, **B, **C, n;

/* rank is the rank of this thread, or k in the formula given */
void *pthr_matbymat(void *arg)
{
  blockLocation *thisBlock = ((ThreadArg *)arg)->thisBlock;

  int i, j;
  for (i = thisBlock->minRow; i < thisBlock->maxRow; i++)
    for (j = thisBlock->minCol; j < thisBlock->maxCol; j++)
      for (int k = 0; k < *n; k++)
        *C[i][j] += (*A[i][k]) * (*B[k][j]);
}
