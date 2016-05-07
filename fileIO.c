#include <stdio.h>
#include <stdlib.h>

#include "fileIO.h"

/*Reads in a matrix and writes to array for output*/
void readFile(char* fileName, int m, int n, int* input)
{
	FILE* f;
	char c;
	int count;
	int position;
	int i;
	char* buffer;

    /*Create buffer to store chars as they are read in for atoi*/
	buffer = (char*)malloc(sizeof(char)*10);

	position = 0;
	count = 0;

    /*Open file in reader mode*/
	f = fopen(fileName, "r");

	do
	{
		c = getc(f);

		/*If not end of number add to buffer*/
		if(((c != ' ') && (c != '\n')) && (c != EOF))
		{
			buffer[count] = c;
			count++;
		}
		else
		{
            /*Convert buffer to int value and add to array*/
			input[position] = atoi(buffer);
			position++;

            /*Set all buffer positions used to null terminator to clear buffer*/
			for(i = 0; i < count; i++)
			{
				buffer[i] = '\0';
			}

			count = 0;
		}

	}
	while(c != EOF);

	fclose(f);

    /*Free buffer array*/
	free(buffer);
}
