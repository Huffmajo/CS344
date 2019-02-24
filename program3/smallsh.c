/***********************************************************
 * Program: smallsh.c
 * Author: Joel Huffman
 * Last updated: 2/24/2019
 * Sources: https://oregonstate.instructure.com/courses/1706555/pages/3-dot-3-advanced-user-input-with-getline
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_CHARS 2048
#define MAX_ARGS 512

struct flag
{
	int background;
	int redirectIn;
	int redirectOut;
	int emptyOrComment;
	int numArgs;
};

// used to track flags used in current operation
struct flag flag;

/***********************************************************
 * Function: GetUserInput()
 * Prompts user for command line arguements and gets entire 
 * string submitted by user. Returns pointer to said string.
 ***********************************************************/
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

/***********************************************************
 * Function: ReadInput(userInput)
 * Takes string input userInput and separates and stores
 * arguements into string array. Checks for redirect in and 
 * out chars while separating and marks flags appropriately.
 * returns a pointer to string array of arguments.
 ***********************************************************/
char** ReadInput(char* userInput)
{
	// maximum amount of arguments is 512
	char** args = malloc(sizeof(char*) * MAX_ARGS);

	int i = 0;
	char* token = strtok(userInput, " \t\r\n");

	// read and separate all arguments in string
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

/***********************************************************
 * Function: ClearFlags()
 * Resets all flags to false (0). No parameters or return 
 * value. 
 ***********************************************************/
void ClearFlags ()
{
	flag.background = 0;
	flag.redirectIn = 0;
	flag.redirectOut = 0;
	flag.emptyOrComment = 0;
	flag.numArgs = 0;
}

/***********************************************************
 * Function: CheckFlags(args)
 * Check command line arguments for remaining flags. Update 
 * flags struct accordingly. Return nothing.
 ***********************************************************/
void CheckFlags (char** args)
{
	// check if blank line or entered as comment
	if (*args == NULL || args[0][0] == '#')
	{
		flag.emptyOrComment = 1;
	}
	// check if background process character
	else if (strcmp(args[flag.numArgs - 1], "&") == 0)
	{
		flag.background = 1;
	}
	else
	{
		
	}
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
		
		// check for remaining flags
		CheckFlags(args);

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
