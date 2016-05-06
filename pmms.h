/*Forward declarations*/
int** buildMatrixArray(int m, int n);
int calcTotal(int m, int n, int k, int* arrayA, int* arrayB);

typedef struct Subtotal
{
	int subtotal;
	int pid;
} Subtotal;