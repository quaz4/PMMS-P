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
#include "fileIO.h"

#define ARRAYA "/arrayA"
#define ARRAYB "/arrayB"
#define ARRAYC "/arrayC"
#define SEMSUB "/sub"

/*Takes in file names of matrix A and B as arguments as well as their dimensions*/
/*execution eg: pmms matrix_A matrix_B M N K*/
int main(int argc, char** argv)
{
	int m;
	int n;
	int k;
	int* a;
	int* b;

	/*Convert paramaters from strings to ints*/
	m = atoi(argv[3]);
	n = atoi(argv[4]);
	k = atoi(argv[5]);

	a = (int*)malloc(sizeof(int)*m*n);
	b = (int*)malloc(sizeof(int)*n*k);

	a = readFile(argv[1], m, n, a);
	b = readFile(argv[2], n, k, b);

	calcTotal(m, n, k, a, b);

	return 0;
}

int calcTotal(int m, int n, int k, int* arrayA, int* arrayB)
{
	int pid;
	int count;
	int temp;
	int i;
	int j;
	int subtotal;
	int total;

	int row;

	/*Create shared memory here before fork
	  to prevent null pointer*/
	int idA;
	int idB;
	int idC;
	int idShared;

	int* sharedArrayA;
	int* sharedArrayB;
	int* sharedArrayC;
	Subtotal* sharedSubtotal;

	int sizeArrayA;
	int sizeArrayB;
	int sizeArrayC;

	sem_t *empty;
	sem_t *full;


	key_t semKey;

	semKey = 123;

	/*Calculate size of shared memory required				FIX THIS MESS*/
	sizeArrayA = m * sizeof(int) * n;
	sizeArrayB = n * sizeof(int) * k;
	sizeArrayC = m * sizeof(int) * k;

	/*Create shared memory blocks							add error checking to these things later*/
	idA = shmget(1234, sizeArrayA, IPC_CREAT | 0666);
	idB = shmget(2234, sizeArrayB, IPC_CREAT | 0666);
	idC = shmget(3234, sizeArrayC, IPC_CREAT | 0666);
	idShared = shmget(4234, sizeof(Subtotal), IPC_CREAT | 0666);

	/*Create Semaphores*/
	empty = sem_open ("empty", O_CREAT | O_EXCL, 0644, 1);
	full = sem_open ("full", O_CREAT | O_EXCL, 0644, 0);

	/*Unlink prevents the semaphore existing forever*/
	sem_unlink ("empty"); 
	sem_unlink ("full"); 

	count = 0;

	/*Attach memory sections for parent*/
	sharedArrayA = (int*)shmat(idA, NULL, 0);
	sharedArrayB = (int*)shmat(idB, NULL, 0);
	sharedArrayC = (int*)shmat(idC, NULL, 0);
	sharedSubtotal = (Subtotal*)shmat(idShared, NULL, 0);

	/*Place contents of arrays in shared memory(Array A)*/
	for(i = 0; i < n*m; i++)
	{
		sharedArrayA[i] = arrayA[i];
		printf("%d\n", arrayA[i]);
		/*sharedArrayB[i] = 0;*/
	}

	/*Place contents of arrays in shared memory(Array B)*/
	for(i = 0; i < n*k; i++)
	{
		sharedArrayB[i] = arrayB[i];
		printf("%d\n", sharedArrayB[i]);
		/*sharedArrayB[i*k + j] = 0;*/
	}

	/*fork m child processes*/
	do
	{
		pid = fork();
		count++;
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
		/*Attach memory sections for parent*/
		sharedArrayA = (int*)shmat(idA, NULL, 0);
		sharedArrayB = (int*)shmat(idB, NULL, 0);
		sharedArrayC = (int*)shmat(idC, NULL, 0);
		sharedSubtotal = (Subtotal*)shmat(idShared, NULL, 0);

		total = 0;

		for(i = 0; i < m; i++)
		{
			sem_wait(full);

			total = total + sharedSubtotal->subtotal;
			printf("Subtotal produced by process with ID %d: %d\n", sharedSubtotal->pid, sharedSubtotal->subtotal);

			sem_post(empty);
		}

		printf("Total: %d\n", total);

		/*Detach shared memory*/

		shmctl(idA, IPC_RMID , 0);
		shmctl(idB, IPC_RMID , 0);
		shmctl(idC, IPC_RMID , 0);
		shmctl(idShared, IPC_RMID , 0);
		
	}
	else /*Child process 								close shared memory*/
	{
		/*Use count from loop where process is forked as row num for matrix multiplication*/
		row = count -1;

		/*Attach memory sections for child*/
		sharedArrayA = (int*)shmat(idA, NULL, 0);
		sharedArrayB = (int*)shmat(idB, NULL, 0);
		sharedArrayC = (int*)shmat(idC, NULL, 0);
		sharedSubtotal = (Subtotal*)shmat(idShared, NULL, 0);

		temp = 0;

		/*Calculate row in array c*/
		for(i = 0; i < k; i++)
		{
			for(j = 0; j < n; j++)
			{
				temp = temp + sharedArrayA[row*n + j] * sharedArrayB[i + j*k];
			}

			sharedArrayC[row*k + i] = temp;
			temp = 0;
		}

		subtotal = 0;

		/*Calculate subtotal*/
		for(i = 0; i < k; i++)
		{
			subtotal = subtotal + sharedArrayC[row*k + i];
		}

		/*Pass subtotal to parent (consumer)*/
		/*Critical Section*/
		sem_wait(empty);

		sharedSubtotal->subtotal = subtotal;
		sharedSubtotal->pid = getpid();

		sem_post(full);

		/*Terminate process*/
		exit(0);
	}

	return 0;
}