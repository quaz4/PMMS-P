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

	int a[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	/*int * a = buildMatrixArray(4, 2);*/
	int b[] = { 1, 2, 3, 4, 5, 6};
	/*int * b = buildMatrixArray(2, 3);*/

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

int calcTotal(int m, int n, int k, int* arrayA, int* arrayB)
{
	int pid;
	int count;

	int i;
	int j;

	/*Create shared memory here before fork
	  to prevent null pointer*/
	int idA;
	int idB;
	int idC;
	int idShared;

	int* sharedArrayA;
	int* sharedArrayB;
	int* sharedArrayC;
	int* sharedSubtotal;

	int sizeArrayA;
	int sizeArrayB;
	int sizeArrayC;

	sem_t* semA;
	sem_t* semB;
	sem_t* semC;
	sem_t* semSub;

	key_t semKey;

	semKey = 123;

	/*Calculate size of shared memory required				FIX THIS MESS*/
	sizeArrayA = m * sizeof(int*) + m * (n * sizeof(int));
	sizeArrayB = n * sizeof(int*) + n * (k * sizeof(int));
	sizeArrayC = m * sizeof(int*) + m * (k * sizeof(int));

	/*Create shared memory blocks							add error checking to these things later*/
	idA = shmget(1234, sizeArrayA, IPC_CREAT | 0666);
	idB = shmget(2234, sizeArrayB, IPC_CREAT | 0666);
	idC = shmget(3234, sizeArrayC, IPC_CREAT | 0666);
	idShared = shmget(4234, sizeof(subtotal), IPC_CREAT | 0666);

	/*Create Semaphores*/
	semA = sem_open(ARRAYA, O_CREAT, 0644, 1);
	semB = sem_open(ARRAYB, O_CREAT, 0644, 1);
	semC = sem_open(ARRAYC, O_CREAT, 0644, 1);
	semSub = sem_open(SEMSUB, O_CREAT, 0644, 1);

	count = 0;

	/*Attach memory sections for parent*/
	sharedArrayA = (int*)shmat(idA, NULL, 0);
	sharedArrayB = (int*)shmat(idB, NULL, 0);
	sharedArrayC = (int*)shmat(idC, NULL, 0);
	sharedSubtotal = (int*)shmat(idShared, NULL, 0);

	/*Place contents of arrays in shared memory*/
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 2; j++)
		{
			sharedArrayA[i*2 + j] = arrayA[i*2 + j];
		}
	}

	/*fork m child processes*/
	do
	{
		pid = fork();
		count++;
		/*printf("%d : %d",pid , count);*/
	}
	while(count < m && pid != 0);

	/*Check for error*/
	if(pid < 0)
	{
		perror("Fork failed\n");
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

		/*Wait for content*/
	}
	else /*Child process*/
	{
		printf("Child\n");

		/*Open semaphores 								check back later if this is needed*/
		semA = sem_open(ARRAYA, 0);
		semB = sem_open(ARRAYB, 0);
		semC = sem_open(ARRAYC, 0);
		semSub = sem_open(SEMSUB, 0);

		/*Attach memory sections for parent*/
		sharedArrayA = (int*)shmat(idA, NULL, 0);
		sharedArrayB = (int*)shmat(idB, NULL, 0);
		sharedArrayC = (int*)shmat(idC, NULL, 0);
		sharedSubtotal = (int*)shmat(idShared, NULL, 0);

		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 2; j++)
			{
				printf("%d - i:%d j:%d\n", sharedArrayA[i*2 + j], i, j);
			}
		}		

		/*Calculate row in array c*/

		/*Calculate subtotal*/

		/*Pass subtotal to parent (consumer)*/

		/*Terminate process*/
	}

	return 0;
}