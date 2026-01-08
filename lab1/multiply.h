#ifndef MULTIPLY_H
#define MULTIPLY_H

typedef struct {
  int row;
  int col;
  int minRow;
  int minCol;
  int maxRow;
  int maxCol;
} blockLocation;

 void *pthr_matbymat(void *arg);

#endif // MULTIPLY_H
