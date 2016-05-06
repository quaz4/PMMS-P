#include <stdio.h>
#include <stdlib.h>

#include "fileIO.h"

int* readFile(char* fileName, int m, int n, int* input)
{
	FILE* f;
	char c;
	int count;
	int currentNum;
	int position;
	int i;

	char* buffer;

	buffer = (char*)malloc(sizeof(char)*10);

	currentNum = 0;
	position = 0;
	count = 0;

	f = fopen(fileName, "r");


	do
	{
		c = getc(f);

		/*If not end of number*/
		if(((c != ' ') && (c != '\n')) && (c != EOF))
		{
			buffer[count] = c;
			count++;
		}
		else
		{
			input[position] = atoi(buffer);
			position++;
			count = 0;

			for(i = 0; i < 10; i++)
			{
				buffer[i] = '\0';
			}
		}

	}
	while(c != EOF);

	fclose(f);

	free(buffer);

	return input;
}