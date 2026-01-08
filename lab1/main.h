#ifndef MAIN_H
#define MAIN_H

#include "multiply.h"

extern int **A, **B, n;

typedef struct {
  blockLocation thisBlock;
} ThreadArg;

#endif // MAIN_H
