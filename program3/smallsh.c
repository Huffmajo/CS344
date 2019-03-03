
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
#include <fcntl.h>

#define MAX_CHARS 2048
#define MAX_ARGS 512
#define _GNU_SOURCE

struct flag
{
	int background;
	int redirectIn;
	int reInFile;
	int redirectOut;
	int reOutFile;
	int emptyOrComment;
	int numArgs;
};

// used to track flags used in current operation
struct flag flag;

// used to track last processed exit status and terminating signal
int lastStatus = -1;
int isStatus;

// limits if processes can be run in background
int backgroundDisable = 0;

// used to queue up background processes and run in order
int bgProcesses[256];
int currBgProcess = 0;

/***********************************************************
 * Function: ExpandPid(string)
 * Expands any $$ substring in string to the process id. 
 * Returns that char*.
 ***********************************************************/
char* ExpandPid(char* string)
{
	int pid = getpid();
	
	while (strstr(string, "$$"))
	{
		// afterStr = strstr(string, "$$") + 2;
		// afterPos = strlen(afterStr);

		// get string after "$$"
		// strncpy(afterStr, string + afterPos, (len - afterPos));
		sprintf(strstr(string, "$$"), "%d", pid);
	}
	
	return string;		
}

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
	
	//expand $$ to pid
	userInput = ExpandPid(userInput);

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
			// next arg is file to write to
			flag.reOutFile = flag.numArgs + 1;
		}

		// if <, set redirectIn flag
		else if (strcmp(token, "<") == 0)
		{
			flag.redirectIn = 1;
			// next arg is file to read from
			flag.reInFile = flag.numArgs + 1;
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
 * Reset all flags to false (0). No parameters or return 
 * value. 
 ***********************************************************/
void ClearFlags ()
{
	flag.background = 0;
	flag.redirectIn = 0;
	flag.reInFile = 0;
	flag.redirectOut = 0;
	flag.reOutFile = 0;
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
	else if (strcmp(args[0], "status") == 0 && args[1] == NULL)
	{
		// check if no prior applicable processes have run
		if (lastStatus == -1)
		{
			printf("exit value 0\n");
			fflush(stdout);
		} 

		// if last applicable process completed, print exit status
		else if (isStatus == 1)
		{
			printf("exit value %d\n", lastStatus);
			fflush(stdout);
		}

		// if last applicable process was terminated by signal, print terminating signal
		else if (isStatus == 0)
		{
			printf("terminated by signal %d\n", lastStatus);
			fflush(stdout);
		}
		else
		{
			printf("Something has gone wrong\n");
			fflush(stdout);
		}
	}
}

/***********************************************************
 * Function: NonBuiltInFunctions(args)
 * Runs any non
 ***********************************************************/
void NonBuiltInFunctions(char** args)
{

	pid_t pid;
	int status;
	int inStatus;
	int outStatus;

	// run in background if it's flagged AND not disabled
	if (flag.background == 1 && backgroundDisable == 0)
	{
		printf("Will run in background\n");
	}

	// otherwise process will run in foreground
	else
	{
		pid = fork();
		// error with fork
		if (pid == -1)
		{
			perror("Fork error");
			exit(1);
		}

		// child process
		else if (pid == 0)
		{
			// redirect stdin if needed
			if (flag.redirectIn == 1)
			{
				// if file is readable, read from it
				if (inStatus = open(args[flag.reInFile], O_RDONLY) != -1)
				{
					// if redirection fails, alert user
					if (dup2(inStatus, 0) == -1)
					{
						printf("Error with stdin redirect\n");
						fflush(stdout);
						exit(1);
					}
				}
		
				// otherwise alert user that file is unreadable
				else
				{
					printf("%s cannot be opened for input\n", args[flag.reInFile]);
					fflush(stdout);
					exit(1);
				}
			}
			
			// redirect stdout if needed
			if (flag.redirectOut == 1)
			{
				// if file doesn't exist, creat it. If it does exists, write to it
				if (outStatus = open(args[flag.reOutFile], O_WRONLY | O_CREAT | O_TRUNC, 0666) == -1)
				{
					printf("%s cannot be opened for output\n", args[flag.reOutFile]);
					fflush(stdout);
				}
				
				// if redirection fails, alert user
				else
				{
					if (dup2(outStatus, 0) == -1)
					{
						printf("Error with stdout redirection\n");
						fflush(stdout);
					}
				}
			}

			// execute function
			if (execvp(*args, args) == -1)
			{
				printf("Execution statement failed\n");
				fflush(stdout);
			}
			exit(0);
		}

		// parent process
		else
		{			
			
		}
	}

	printf("non-built-in funct\n");
}

/***********************************************************
 * Function: SwitchBackgroundMode(signo)
 * Toggles on or off whether input processes can be run in 
 * the background with '&'. Returns nothing.
 ***********************************************************/
void SwitchBackgroundMode(int signo)
{
	char* message = "\nNew processes can no longer be run in background\n";

	// disable if enabled
	if (backgroundDisable == 0)
	{
		// new background processes now disabled
		backgroundDisable = 1;
		// can't use printf due to reentrancy issue
		write(STDOUT_FILENO, message, 50);
	}

	// enable if disabled
	else
	{
		// new background processes now re-enabled
		backgroundDisable = 0;
		message = "\nNew processes can now be run in the background\n";
		write(STDOUT_FILENO, message, 48);
	}
}

int main ()
{
	// setup signal handler for CTRL-C
	struct sigaction SIGINT_action = {0};

	// don't run default terminate
	SIGINT_action.sa_handler = SIG_IGN;

	// setup signal handler for CTRL-Z
	struct sigaction SIGTSTP_action = {0};

	// use function instead of default stop 
	SIGTSTP_action.sa_handler = SwitchBackgroundMode;

	// link signals to use created handlers
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	//run program until exit is entered
	while(1)
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
	}
	return 0;
}
