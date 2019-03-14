/***********************************************************
 * Program: keygen.c
 * Author: Joel Huffman
 * Last updated: 3/9/2019
 * Description: Generates and returns to stdout a string of
 * random characters from A-Z and space of length determined
 * by the user.
 * Sources: https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/***********************************************************
 * Function: stderror(err)
 * Accepts a string. Sends that string to standard error and
 * exits with value 1.
 ***********************************************************/
void stderror(char* err)
{
	fprintf(stderr, err);
	exit(1);
}

int main (int argc, char* argv[])
{
	int keylength;
	int random;

	// seed random
	srand(time(NULL));

	// input should follow "keygen keylength" syntax
	if (argc == 2) 
	{
		// check if provided keylength is a valid integer
		if ((keylength = atoi(argv[1])) && keylength > 0)
		{

			// print chars equal to keylength
			int i;
			for (i = 0; i < keylength; i++)
			{
				// get a random character A-Z or space
				random = 65 + rand() % 27;

				// use 91 as reserve for space char
				if (random == 91)
				{
					random = 32;
				}

				// print char to stdout
				printf("%c", random);
			}

			// print trailing newline to stdout
			printf("\n");
		}

		// otherwise keylength is invalid
		else
		{
			stderror("Keylength is not a valid integer\n");
			exit(1);
		}
	}

	// otherwise wrong number of arguments
	else
	{
		stderror("Wrong number of arguments\n");
		exit(1);
	}

	return 0;
}
