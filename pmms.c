#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>

#include "pmms.h"

#define ARRAYA "/arrayA"
#define ARRAYB "/arrayB"
#define ARRAYC "/arrayC"
#define SEMSUB "/sub"

/*Takes in file names of matrix A and B as arguments as well as their dimensions*/
/*pmms matrix_A matrix_B M N K*/
int main(int argc, char** argv)
{
	int m;
	int n;
	int k;

	/*int a[4][2] = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };*/
	int ** a = buildMatrixArray(4, 2);
	/*int b[2][3] = { {1, 2, 3}, {4, 5, 6}};*/
	int ** b = buildMatrixArray(2, 3);

	int i, j;

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 2; j++)
		{
			a[i][j] = j;
		}
	}

	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 3; j++)
		{
			b[i][j] = j;
		}
	}

	/*Convert paramaters from strings to ints*/
	m = atoi(argv[3]);
	n = atoi(argv[4]);
	k = atoi(argv[5]);

	/*Debug: Display params*/
	printf("%s, %s\n", argv[1], argv[2]);
	printf("%d - %d - %d\n", m, n, k);

	calcTotal(m, n, k, a, b);

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
	int count;

	/*Create shared memory here before fork
	  to prevent null pointer*/
	int sharedArrayA;
	int sharedArrayB;
	int sharedArrayC;
	int sharedSubtotal;
	int sizeArrayA;
	int sizeArrayB;
	int sizeArrayC;

	sem_t* semA;
	sem_t* semB;
	sem_t* semC;
	sem_t* semSub;

	key_t semKey;

	semKey = 123;

	/*Calculate size of shared memory required*/
	sizeArrayA = m * sizeof(int*) + m * (n * sizeof(int));
	sizeArrayB = n * sizeof(int*) + n * (k * sizeof(int));
	sizeArrayC = m * sizeof(int*) + m * (k * sizeof(int));

	/*Create shared memory blocks*/
	sharedArrayA = shmget(1, sizeArrayA, IPC_CREAT);
	sharedArrayB = shmget(2, sizeArrayB, IPC_CREAT);
	sharedArrayC = shmget(3, sizeArrayC, IPC_CREAT);
	sharedSubtotal = shmget(4, sizeof(subtotal), IPC_CREAT);

	/*Create Semaphores*/
	semA = sem_open(ARRAYA, O_CREAT, 0644, 1);
	semB = sem_open(ARRAYB, O_CREAT, 0644, 1);
	semC = sem_open(ARRAYC, O_CREAT, 0644, 1);
	semSub = sem_open(SEMSUB, O_CREAT, 0644, 1);

	count = 0;

	/*fork m child processes*/
	do
	{
		pid = fork();
		count++;
		printf("%d : %d",pid , count);
	}
	while(count < m && pid != 0);

	/*Check for error*/
	if(pid < 0)
	{
		perror("Fork failed");
		exit(-1);
	}
	else if(pid != 0) /*Parent process*/
	{
		printf("Parent\n");
		/*Open semaphores 								check back later if this is needed*/
		semA = sem_open(ARRAYA, 0);
		semB = sem_open(ARRAYB, 0);
		semC = sem_open(ARRAYC, 0);
		semSub = sem_open(SEMSUB, 0);

		/*Wait for */
	}
	else /*Child process*/
	{
		printf("Child\n");

		/*Open semaphores 								check back later if this is needed*/
		semA = sem_open(ARRAYA, 0);
		semB = sem_open(ARRAYB, 0);
		semC = sem_open(ARRAYC, 0);
		semSub = sem_open(SEMSUB, 0);

		/*Calculate row in array c*/

		/*Calculate subtotal*/

		/*Pass subtotal to parent (consumer)*/

		/*Terminate process*/
	}

	return 0;
}