/***********************************************************
 * Program: smallsh.c
 * Author: Joel Huffman
 * Last updated: 3/3/2019
 * Sources: https://oregonstate.instructure.com/courses/1706555/pages/3-dot-3-advanced-user-input-with-getline
 * https://www.geeksforgeeks.org/dup-dup2-linux-system-call/
 * 
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_CHARS 2048
#define MAX_ARGS 512
#define _GNU_SOURCE

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

// used to track last processed exit status or terminating signal
char* lastStatus;

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

	// expand $$ to pid
	// char* afterStr = NULL;
	// int afterPos;
	// int inputLen;


	while (strstr(userInput, "$$"))
	{
		// afterStr = strstr(userInput, "$$") + 2;
		// afterPos = strlen(afterStr);
		// inputLen = strlen(userInput) - 1;

		// get string after "$$"
		// strncpy(afterStr, userInput + afterPos, (len - afterPos));
		sprintf(strstr(userInput, "$$"), "%d", getpid());
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

	// split string by any spacing characters
	char* token = strtok(userInput, " \t\r\n");

	// read and separate all arguments in string
	while (token != NULL)
	{
		// if >, set redirectOut flag
		if (strcmp(token, ">") == 0)
		{
			flag.redirectOut = 1;
		}
		// if <, set redirectIn flag
		else if (strcmp(token, "<") == 0)
		{
			flag.redirectIn = 1;
		}
		// otherwise it's a command, argument or '&'
		else
		{			
			args[flag.numArgs] = token;
			flag.numArgs++;

		}
		token = strtok(NULL, " \t\r\n");
	}
	// mark end of args array
	args[flag.numArgs] = NULL;
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
	if (args[0] == NULL || strncmp(args[0], "#", 1) == 0)
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
		// do nothing
	}
}

/***********************************************************
 * Function: BuiltInFunctions(args)
 * Runs one of the built-in commands based on command line 
 * user input in args. Can run "exit", "cd" and "status". 
 * Returns nothing.
 ***********************************************************/
void BuiltInFunctions(char** args)
{
	// check if built-in exit function is called, & is accepted but ignored
	if (strcmp(args[0], "exit") == 0 && (args[1] == NULL || strcmp(args[1], "&") == 0))
	{
		// kill all processes

		// exit shell
		exit(0);
	}
	// check if built-in cd function is called
	else if (strcmp(args[0], "cd") == 0)
	{
		// if only arg is "cd" go to home dir
		if (flag.numArgs == 1)
		{
			chdir(getenv("HOME"));
		}
		// try to change dir to given arguement
		else if (chdir(args[1]) == 0)
		{
			// directory successfully changes
		}
		// if not, print error message for user
		else
		{
			printf("%s is not a valid directory\n", args[1]);
			fflush(stdout);
		}
	}
	// otherwise must be status function 
	else if (strcmp(args[0], "status") == 0)
	{
		printf("Status built-in here\n");
	}
}

/***********************************************************
 * Function: NonBuiltInFunctions(args)
 * Runs any non
 ***********************************************************/
void NonBuiltInFunctions(char** args)
{
/*
	pid_t pid;
	int status;
	// check if process will be run in foregorund
	if (flag.background == 0)
	{

	}
	// otherwise process will be run in background
	else 
	{

	}
*/
	printf("Non-built-in function here\n");

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

		// check if input is not a comment or empty
		if (flag.emptyOrComment == 0)
		{
			// check if input is a built-in function
			if ((strcmp(args[0], "exit") == 0) || (strcmp(args[0], "cd") == 0) || (strcmp(args[0], "status") == 0))
			{
				BuiltInFunctions(args);
			}
			
			// otherwise run it as a non-built-in function
			else
			{
				NonBuiltInFunctions(args);
			}
		}

		int i;
		for (i = 0; i < flag.numArgs; i++)
		{
			printf("%s ", args[i]);
		}
		printf("\n");

/*
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
		if (flag.emptyOrComment == 1)
		{
			printf("emptyOrComment\n");
		}
		printf("Number of arguments: %d\n", flag.numArgs);

		printf("\nArguments\n");
		int i = 0;
		for (i = 0; i < flag.numArgs; i++)
		{
			printf("%d: %s\n", i, args[i]);
		}
*/

			

		// make sure to exit loop
		keepRunning = 0;		
	}


	return 0;
}
