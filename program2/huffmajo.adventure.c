/***********************************************************
 * Program: huffmajo.adventure.c
 * Author: Joel Huffman
 * Last updated: 2/5/2019
 * Sources: https://oregonstate.instructure.com/courses/1706555/pages/2-dot-4-manipulating-directories
 * 
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

#define MAX_LINKS 6
#define NUM_ROOMS 7

struct room
{
	char* name;
	char* type;
	int numlinks;
	struct room* link[MAX_LINKS];
};

struct room  chosenrooms[NUM_ROOMS];

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
 * Function: GetRoomData()
 * Pulls the name, connections and type of each room from the
 * roomfiles contained in the latest local directory.
 ***********************************************************/
void GetRoomData()
{
	// find the latest directory of room files
	GetLatestDir();

	// search that directory for room files

	// for each room file, store name, connections and type in chosenrooms array

}


int main ()
{
	// get latest directory
	GetLatestDir();
	
	// get data from room files

	return 0;
}
