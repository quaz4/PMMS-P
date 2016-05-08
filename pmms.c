/*
    Created by William Stewart 18349788
    A program that calculates the matrix multiplication sum of two matrices
*/

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

    /*Allocate memory for matrix arrays*/
    /*Uses logical 2D array so can be used in shared memory*/
	a = (int*)malloc(sizeof(int)*m*n);
	b = (int*)malloc(sizeof(int)*n*k);
    
    /*Read files and assign to arrays*/
	readFile(argv[1], m, n, a);
	readFile(argv[2], n, k, b);

	calcTotal(m, n, k, a, b);

	return 0;
}

/*Using m processes, calculates the matrix multiplication sum*/
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

    /*Shared memory pointers*/
	int* sharedArrayA;
	int* sharedArrayB;
	int* sharedArrayC;
	Subtotal* sharedSubtotal;

    /*Will store array sizes*/
	int sizeArrayA;
	int sizeArrayB;
	int sizeArrayC;

    /*Declare named semaphores*/
	sem_t *empty;
	sem_t *full;

	/*Calculate size of shared memory required*/
	sizeArrayA = m * sizeof(int) * n;
	sizeArrayB = n * sizeof(int) * k;
	sizeArrayC = m * sizeof(int) * k;

	/*Create shared memory blocks*/
	idA = shmget(1234, sizeArrayA, IPC_CREAT | 0666);
	idB = shmget(2234, sizeArrayB, IPC_CREAT | 0666);
	idC = shmget(3234, sizeArrayC, IPC_CREAT | 0666);
	idShared = shmget(4234, sizeof(Subtotal), IPC_CREAT | 0666);

    /*Check for erros in shmget (error on ptr == -1)*/
    if(idA == -1 || idB == -1 || idC == -1)
    {
        perror("Error creating shared memory blocks");
    }

	/*Create Semaphores*/
	empty = sem_open("empty", O_CREAT | O_EXCL, 0644, 1);
	full = sem_open("full", O_CREAT | O_EXCL, 0644, 0);

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
	}

	/*Free array a as soon as possible*/
	free(arrayA);

	/*Place contents of arrays in shared memory(Array B)*/
	for(i = 0; i < n*k; i++)
	{
		sharedArrayB[i] = arrayB[i];
	}
	
	/*Free arrayB as soon as possible*/
	free(arrayB);

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
		total = 0;

        /*Calculate subtotals, then calculate total, output both subtotals and total*/
		for(i = 0; i < m; i++)
		{
            /*Wait until subtotal full*/
			sem_wait(full);

			total = total + sharedSubtotal->subtotal;
			printf("Subtotal produced by process with ID %d: %d\n", sharedSubtotal->pid, sharedSubtotal->subtotal);

            /*Signal if subtotal empty*/
			sem_post(empty);
		}

		printf("Total: %d\n", total);

		/*Mark for detach*/
		shmctl(idA, IPC_RMID , 0);
		shmctl(idB, IPC_RMID , 0);
		shmctl(idC, IPC_RMID , 0);
		shmctl(idShared, IPC_RMID , 0);

        /*Detach shared memory*/
		shmdt(sharedArrayA);
		shmdt(sharedArrayB);
		shmdt(sharedArrayC);
		shmdt(sharedSubtotal);

		/*Unlink prevents the semaphore existing forever*/
		sem_unlink ("empty"); 
		sem_unlink ("full"); 
	}
	else /*Child process*/
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

        /*Wait until empty*/
		sem_wait(empty);

		sharedSubtotal->subtotal = subtotal;
		sharedSubtotal->pid = getpid();

        /*Signal full*/
		sem_post(full);

		/*Terminate child process*/
		exit(0);
	}

	return 0;
}