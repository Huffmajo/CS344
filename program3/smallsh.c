/***********************************************************
 * Program: smallsh.c
 * Author: Joel Huffman
 * Last updated: 2/23/2019
 * Sources: 
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_CHARS 2048
#define MAX_ARGS 512

// char input[MAX_CHARS];

struct flag
{
	int background;
	int redirectIn;
	int redirectOut;
	int numArgs;
};

struct flag flag;

char* GetUserInput()
{
	char* userInput = NULL;
	size_t bufferSize = 0;

	// prompt user with :
	printf(": ");
	fflush(stdout);

	// clear error if interrupted mid-read
	if (getline(&userInput, &bufferSize, stdin) == -1)
	{
		clearerr(stdin);
	}

	return userInput;
}

char** ReadInput(char* userInput)
{
	// maximum amount of arguments is 512
	char** args = malloc(sizeof(char*) * MAX_ARGS);
	
	int i = 0;
	char* token = strtok(userInput, " \t\r\n");

	// read all arguments from 	
	while (token != NULL)
	{
		if (strcmp(token, ">") == 0)
		{
			flag.redirectOut = 1;
		}
		else if (strcmp(token, "<") == 0)
		{
			flag.redirectIn = 1;
		}
		else
		{
			args[i] = token;
			i++;
		}
		token = strtok(NULL, " \t\r\n");
	}
	// mark end of args array
	args[i] = NULL;
	flag.numArgs = i;
	return args;
}

void ClearFlags ()
{
	flag.background = 0;
	flag.redirectIn = 0;
	flag.redirectOut = 0;
	flag.numArgs = 0;
}

int main ()
{
	int keepRunning = 1;

	//run program 
	while(keepRunning)
	{
		// get input from command line
		char* userInput = GetUserInput();

		// set all flags to false
		ClearFlags();

		// read user input word by word and set flags
		char** args = ReadInput(userInput);
		
		// run functions based on flags and commands
		printf("Flags raised:\n");
		if (flag.background == 1)
		{
			printf("background\n");
		}
		if (flag.redirectIn == 1)
		{
			printf("redirectIn (<)\n");
		}
		if (flag.redirectOut == 1)
		{
			printf("redirectOut (>)\n");
		}
		printf("Number of arguments: %d\n", flag.numArgs);

		printf("\nArguments\n");
		int i = 0;
		for (i = 0; i < flag.numArgs; i++)
		{
			printf("%d: %s\n", i, args[i]);
		}

		// make sure to exit loop
		keepRunning = 0;		
	}


	return 0;
}
