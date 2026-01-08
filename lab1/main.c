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