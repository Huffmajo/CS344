/***********************************************************
 * Program: huffmajo.adventure.c
 * Author: Joel Huffman
 * Last updated: 2/9/2019
 * Sources: https://oregonstate.instructure.com/courses/1706555/pages/2-dot-4-manipulating-directories
 * https://www.youtube.com/watch?v=nlHIuG3RQ0g
 * https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

#define MAX_LINKS 6
#define NUM_ROOMS 7

struct room
{
	char* name;
	char* type;
	int numlinks;
	char link[MAX_LINKS][10];
	char* filename;
};

struct room chosenrooms[NUM_ROOMS];


char newestDirName[256];
//memset(newestDirName, '\0', sizeof(newestDirName));

/***********************************************************
 * Function: GetLatestDir()
 * Compares all directories in the current directory and
 * returns the name of the most recently created one. Code is
 * a modified version of Ben Brewster's readDirectory.c file. 
 * Link to source is available in program header source section.
 ***********************************************************/
void GetLatestDir()
{
	int newestDirTime = -1;
	char targetDirPrefix[32] = "huffmajo.rooms.";

	DIR* dirToCheck;
	struct dirent* fileInDir;
	struct stat dirAttributes;

	// open current directory
	dirToCheck = opendir(".");

	if(dirToCheck > 0)
	{
		while ((fileInDir = readdir(dirToCheck)) != NULL)
		{
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
//				printf("Found the prefix: %s\n", fileInDir->d_name);
				stat(fileInDir->d_name, &dirAttributes);

				if ((int)dirAttributes.st_mtime > newestDirTime)
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
//					printf("Newer subdir: %s, new time: %d\n", fileInDir->d_name, newestDirTime);
				}
			}
		}
	}
	closedir(dirToCheck);
//	printf("Newest entry found is: %s\n", newestDirName);
}
	
/***********************************************************
 * Function: GetFileNames()
 * Pulls files names from sourced directory and stores them
 * in chosenrooms array.
 ***********************************************************/
void GetFileNames()
{
	// get file names for rooms and store in chosenrooms.filename
	DIR* dirToCheck = opendir(newestDirName);
	struct dirent* fileInDir;
	FILE* roomfile;
	char roomfiledir[50];
	int filenum = 0;

	if(dirToCheck > 0)
	{
		// check each file
		while ((fileInDir = readdir(dirToCheck)) != NULL)
		{
			if (strcmp(fileInDir->d_name, ".") != 0 && strcmp(fileInDir->d_name, "..") != 0)
			{
				chosenrooms[filenum].filename = malloc(20 * sizeof(char));
				strcpy(chosenrooms[filenum].filename, fileInDir->d_name);
//				printf("File %d: %s\n", filenum, chosenrooms[filenum].filename);
				filenum++;
			}
		}
	}
	closedir(dirToCheck);
}

/***********************************************************
 * Function: ReadRoomFiles()
 * Copies room name, room type and connections to local array.
 ***********************************************************/
void ReadRoomFiles()
{
	FILE* myfile;
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		// string for directory name
		char filedir[50];
		memset(filedir, '\0', sizeof(filedir));

		// open room file
		sprintf(filedir, "./%s/%s", newestDirName, chosenrooms[i].filename);
		myfile = fopen(filedir, "r");

		// buffer string for reading
		char buffer[50];
		memset(buffer, '\0', sizeof(buffer));

		// read file one line at a time
		while (fgets(buffer, 50, myfile))
		{
			char secondread[15];
			memset(secondread, '\0', sizeof(secondread));
			char thirdread[15];
			memset(thirdread, '\0', sizeof(thirdread));

			sscanf(buffer, "%*s %s %s", secondread, thirdread);

			// check if second word in line is "NAME:" for room name
			if (strcmp(secondread, "NAME:") == 0)
			{
				strcpy(chosenrooms[i].name, thirdread);
			}

			// check if second word is "TYPE:" for room type
			else if (strcmp(secondread, "TYPE:") == 0)
			{
				strcpy(chosenrooms[i].type, thirdread);
			}
			
			// must be a connection line
			else
			{
				strcpy(chosenrooms[i].link[chosenrooms[i].numlinks], thirdread);
				chosenrooms[i].numlinks++;				
			}			

		}

		// close file when done reading from it
		fclose(myfile);
	}
}

/***********************************************************
 * Function: AllocateMem()
 * Malloc's memory to persistent objects that require it.
 ***********************************************************/
void AllocateMem()
{
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		chosenrooms[i].name = malloc(10 * sizeof(char));
		chosenrooms[i].type = malloc(16 * sizeof(char));
	}	
}

/***********************************************************
 * Function: GetRoomData()
 * Pulls the name, connections and type of each room from the
 * roomfiles contained in the latest local directory.
 ***********************************************************/
void GetRoomData()
{
	// find the latest directory of room files
	GetLatestDir();

	// allocate memory for chosenroom's attributes
	AllocateMem();	

	// get filenames for all room files
	GetFileNames();

	// for each room file, store name, connections and type in chosenrooms array
	ReadRoomFiles();


	// test print
/*
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		printf("Name: %s\n", chosenrooms[i].name);
		int j;
		for (j=0; j<chosenrooms[i].numlinks; j++)
		{
			printf("Con: %s\n", chosenrooms[i].link[j]);
		}
		printf("Type: %s\n\n", chosenrooms[i].type);
	}
*/
}

/***********************************************************
 * Function: WriteTime()
 * Gets current time and writes (or overwrites) 
 * currentTime.txt.
 ***********************************************************/
void* WriteTime()
{
	time_t rawtime;
	struct tm *info;
	char buffer[80];
	memset(buffer, '\0', sizeof(buffer));

	time(&rawtime);
	info = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%I:%M%p, %A, %B %d, %Y", info);

	FILE* writefile = fopen("currentTime.txt", "w");
	fprintf(writefile, "%s\n", buffer);
	fclose(writefile);
}

/***********************************************************
 * Function: ReadTime()
 * Reads time data from currentTime.txt and prints it for the
 * user to see.
 ***********************************************************/
void ReadTime()
{
	char buffer[50];
	memset(buffer, '\0', sizeof(buffer));

	FILE* readfile = fopen("currentTime.txt", "r");
	fgets(buffer, sizeof(buffer), readfile);
	printf("\n%s\n", buffer);
	fclose(readfile);
}

/***********************************************************
 * Function: RunGame()
 * Manages all facets of the game including user I/O, victory
 * case management and positioning.
 ***********************************************************/
void RunGame()
{
	// initialize mutex to lock/unlock threads
	pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

	// establish starting variables
	int currentpos;
	int steps = 0;
	int roomsvisited[256];
	char buffer[256];
	memset(buffer, '\0', sizeof(buffer));
	int pickedroom;

	// start player in the START_ROOM
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		if (strcmp(chosenrooms[i].type, "START_ROOM") == 0)
		{
			currentpos = i;
			break;
		}
	}

	// keep playing until victory condition met
	while (strcmp(chosenrooms[currentpos].type, "END_ROOM") != 0)
	{
		// print current locale and movement options
		printf("CURRENT LOCATION: %s\n", chosenrooms[currentpos].name);
		printf("POSSIBLE CONNECTIONS: ");
		
		i = 0;
		for (i=0; i<chosenrooms[currentpos].numlinks; i++)
		{
			printf(chosenrooms[currentpos].link[i]);
			
			//insert period and newline if last connection
			if (i == (chosenrooms[currentpos].numlinks -1))
			{
				printf(".\n");
			}

			// insert comma and space if more connections to come
			else
			{
				printf(", ");
			}
		}

		// ask user which room to move to
		printf("WHERE TO? >");

		// get input from user
		scanf("%s", buffer);

		int validinput = 0;

		// check if input is a listed connection
		i = 0;
		for (i=0; i<chosenrooms[currentpos].numlinks; i++)
		{
			if (strcmp(buffer, chosenrooms[currentpos].link[i]) == 0)
			{
				validinput = 1;
				
				// need to find matching index according to chosenrooms
				int j;
				for (j=0; j<NUM_ROOMS; j++)
				{
					if (strcmp(buffer, chosenrooms[j].name) == 0)
					{
						pickedroom = j;
					}
				}				
			}
		}
		
		// check if user is looking for time function
		if (strcmp(buffer, "time") == 0)
		{
			// create separate thread for time-keeping
			pthread_t timethread;

			// only let one thread write to currentTime.txt file
			pthread_mutex_lock(&mymutex);
			int resultcode = pthread_create(&timethread, NULL, WriteTime, NULL);
			pthread_mutex_unlock(&mymutex);

			// block main thread while reading currentTime.txt
			resultcode = pthread_join(timethread, NULL);
			
			// only let one thread read currentTime.txt file
			pthread_mutex_lock(&mymutex);
			ReadTime();
			pthread_mutex_unlock(&mymutex);
		}

		// if neither of the above, let user try again
		else if (validinput == 0)
		{
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}

		// otherwise move rooms and take note of the updated path
		else
		{
			// move to chosen room	
			currentpos = pickedroom;

			// record room traveled to
			roomsvisited[steps] = currentpos;

			// increment # of steps
			steps++;
			printf("\n");
		}
	}
	
	// print congratulations message
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	
	// print each visited room
	i = 0;
	for (i=0; i<steps; i++)
	{
		printf("%s\n", chosenrooms[roomsvisited[i]].name);
	}

	// destroy mutex
	pthread_mutex_destroy(&mymutex);
}

/***********************************************************
 * Function: FreeAtLast()
 * Frees all previously allocated memory to prevent memory
 * Leaks. Also honors the late, great MLK Jr.
 ***********************************************************/
void FreeAtLast()
{
	// free room names and types
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		free(chosenrooms[i].filename);
		free(chosenrooms[i].name);
		free(chosenrooms[i].type);
	}
}

int main ()
{
	// get data from room files
	GetRoomData();
	
	// Initiate and run the game
	RunGame();

	// free up memory
	FreeAtLast();

		
	return 0;
}
