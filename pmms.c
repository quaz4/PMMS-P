#include <stdlib.h>
#include <stdio.h>

/*Takes in file names of matrix A and B as arguments as well as their dimensions*/
/*pmms matrix_A matrix_B M N K*/
int main(int argc, char** argv)
{
	int m;
	int n;
	int k;

	/*Convert paramaters from strings to ints*/
	m = atoi(argv[3]);
	n = atoi(argv[4]);
	k = atoi(argv[5]);

	/*Debug: Display params*/
	printf("%s, %s\n", argv[1], argv[2]);
	printf("%d - %d - %d\n", m, n, k);

	return 0;
}

/*Builds an m*n "matrix" and returns a pointer to it*/
int** buildMatrixArray(int m, int n)
{
	int** array;
	int i;
	
	/*Rows*/
	array = (int**)malloc(sizeof(int*)*m);

	/*Cols*/
	for(i = 0; i < m; i++) 
	{
		array[i] = (int*)malloc(sizeof(int)*n);
	}

	return array;
}

int calcTotal(int m, int n, int k, int** arrayA, int** arrayB)
{
	int pid;
	pid = fork();

	/*Check for error*/
	if(pid < 0)
	{
		perror("Fork failed");
		exit(-1);
	}
	else if(pid != 0) /*Parent process*/
	{
		
	}
	else /*Child process*/
	{

	}
}