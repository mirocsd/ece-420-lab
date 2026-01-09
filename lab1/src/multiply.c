#include <stdlib.h>

#include "main.h"
#include "multiply.h"

extern long p;
extern int **A, **B, **C, n;

/* rank is the rank of this thread, or k in the formula given */
void *thr_matbymat(void *arg)
{
  blockLocation *thisBlock = ((ThreadArg *)arg)->thisBlock;

  int i, j;
  long long sum;
  for (i = thisBlock->minRow; i < thisBlock->maxRow; i++)
    for (j = thisBlock->minCol; j < thisBlock->maxCol; j++) {
      sum = 0;

      for (int k = 0; k < n; k++)
        sum += (long long) (A[i][k]) * (B[k][j]);

      C[i][j] = (int)sum;
    }
  return NULL;
}
