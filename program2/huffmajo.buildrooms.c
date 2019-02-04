/***********************************************************
 * Program: huffmajo.buildrooms.c
 * Author: Joel Huffman
 * Last updated: 2/2/2019
 * Sources: https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
 * https://oregonstate.instructure.com/courses/1706555/pages/2-dot-2-program-outlining-in-program-2
 * 
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define TOTAL_ROOMS 10
#define NUM_ROOMS 7
#define ROOM_TYPES 3
#define MIN LINKS 3
#define MAX_LINKS 6

// define some room necessities
char* roomtypes[ROOM_TYPES] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
char* roomnames[TOTAL_ROOMS] = {"RED", "ORANGE", "YELLOW", "GREEN", "BLUE",
				"INDIGO", "VIOLET", "BLACK", "WHITE", "GREY"};

struct room
{
	int id;
	char* name;
	char* type;
	int numlinks;
	struct room* link[MAX_LINKS];
};

struct room chosenrooms[NUM_ROOMS];


/***********************************************************
 * Function: fisherYatesRand(arr[], n)
 * Accepts an array of ints and the size of that array. Arr 
 * is shuffled randomly. Returns nothing
 ***********************************************************/
void fisherYatesRand (int arr[], int n)
{
	int i;
	for (i= n-1; i > 0; i--)
	{
		int j = rand() % (i+1);
		int temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	} 
}

/***********************************************************
 * Function: CreateRoomsDir()
 * Creates the directory to hold the room files. Includes the
 * pid in the directory name.
 ***********************************************************/
void CreateRoomsDir ()
{
	// create directory with current PID
	char roomsdir[25];
	memset(roomsdir, '\0', 25);
	sprintf(roomsdir, "huffmajo.rooms.%d", getpid());
	mkdir(roomsdir, 0755);
}

/***********************************************************
 * Function: ChooseRooms()
 * Selects 7 room names at random from the set of 10 created.
 * Creates a room struct for each and popualtes with the 
 * name. The first 
 ***********************************************************/
void ChooseRooms ()
{
	// randomize arr of ints from 0-9
	int roomnum[TOTAL_ROOMS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	fisherYatesRand(roomnum, TOTAL_ROOMS);
	
	// pull first 7 rooms from our total of 10
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		// allocate space for all the room values
		chosenrooms[i].name = malloc(10 * sizeof(char));
		chosenrooms[i].type = malloc(16 * sizeof(char));	

		// all rooms start with 0 links and get named
		chosenrooms[i].numlinks = 0;
		strcpy(chosenrooms[i].name, roomnames[roomnum[i]]);
		
		// first random room is START_ROOM
		if (i == 0)
		{
			strcpy(chosenrooms[i].type, roomtypes[0]);
		}
		// second random room is END_ROOM
		else if (i == (NUM_ROOMS-1))
		{
			strcpy(chosenrooms[i].type, roomtypes[1]);
		}
		// all other rooms are MID_ROOM
		else
		{
			strcpy(chosenrooms[i].type, roomtypes[2]);
		}

		//test print
		printf("%s: %s\n", chosenrooms[i].type, chosenrooms[i].name);
	}
}


void FreeAtLast()
{
	// free room names and types
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		free(chosenrooms[i].name);
		free(chosenrooms[i].type);
	}
}


int main ()
{
	// seed randomization
	srand(time(NULL));

	// create dir for room files
	CreateRoomsDir();

	// Create and assign random rooms
	ChooseRooms();
	
	// Links chosen rooms randomly

	// generate room files

	// free any allocated memory
	FreeAtLast();
	return 0;
}
