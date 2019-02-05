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
#define MIN_LINKS 3
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
 * Initializes rooms with 0 linked rooms, names and types.
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

/***********************************************************
 * Function: IsGraphFull()
 * Checks if all the rooms have the minimum number of links
 * to other rooms. Returns 1 if this is that case, 0 if 
 * otherwise.
 ***********************************************************/
int IsGraphFull()
{
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		if (chosenrooms[i].numlinks < MIN_LINKS)
		{
			return 0;
		}
	}
	return 1;
}



/***********************************************************
 * Function: GetRandomRoom()
 * Returns a random room, does not validate if link is valid.
 ***********************************************************/
/*
struct room* GetRandomRoom()
{
	int randnum = rand() % NUM_ROOMS;
	struct room* randroom = &chosenrooms[randnum];
	return randroom;
}
*/

/***********************************************************
 * Function: CanAddConnectionFrom(r) 
 * Checks if room r can handle another link. Returns 1 if
 *  possible, 0 if link limit is already reached.
 ***********************************************************/
int CanAddConnectionFrom(int n)
{
	if (chosenrooms[n].numlinks >= MAX_LINKS)
		return 0;
	else
		return 1;
}

/***********************************************************
 * Function: ConnectionAlreadyExists(a, b)
 * Checks if rooms a and b are already connected. If so, 
 * returns 1, else returns 0.
 ***********************************************************/
int ConnectionAlreadyExists(int a, int b)
{
	// check for no links
	if (chosenrooms[a].numlinks == 0 || chosenrooms[b].numlinks == 0)
	{
		return 0;
	}

	int i;
	for (i=0; i<(chosenrooms[a].numlinks); i++)
	{
		if (strcmp(chosenrooms[a].link[i]->name, chosenrooms[b].name) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/***********************************************************
 * Function: ConnectRoom(a, b)
 * Connects room a to room b and room b to room a.
 ***********************************************************/
void ConnectRoom(int a, int b)
{
	chosenrooms[a].link[chosenrooms[a].numlinks] = &chosenrooms[b];
	chosenrooms[a].numlinks++;
}

/***********************************************************
 * Function: IsSameRoom(a, b)
 * Checks if a and b are the same room. Returns 1 if so, 0
 * otherwise.
 ***********************************************************/
int IsSameRoom(int a, int b)
{

	if (strcmp(chosenrooms[a].name, chosenrooms[b].name) == 0)
		return 1;
	else
		return 0;
}

/***********************************************************
 * Function: AddRandomConnection()
 * Adds a random, valid connection from one room to another.
 ***********************************************************/
void AddRandomConnection() 
{
	int a = rand() % NUM_ROOMS;
	int b = rand() % NUM_ROOMS;

	while (CanAddConnectionFrom(a) == 0)
	{
		a = rand() % NUM_ROOMS;
	}

	while (CanAddConnectionFrom(b) == 0 || IsSameRoom(a, b) == 1 || ConnectionAlreadyExists(a, b) == 1)
	{
		b = rand() % NUM_ROOMS;
	}

	ConnectRoom(a, b);
	ConnectRoom(b, a);
}

/***********************************************************
 * Function: LinkRooms()
 * Randomly links rooms to one another until all rooms are 
 * sufficiently paired.
 ***********************************************************/
void LinkRooms() {
	while (IsGraphFull() == 0)
	{
		AddRandomConnection();
	}
	
}

/***********************************************************
 * Function: FreeAtLast()
 * Frees all previously allocated memory to prevnet memory
 * Leaks. Also honors the late, great MLK Jr.
 ***********************************************************/
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
	srand(time(0));

	// create dir for room files
	CreateRoomsDir();

	// Create and assign random rooms
	ChooseRooms();
	
	// Link chosen rooms randomly
	LinkRooms();

	// Test print
	int i;
	for (i=0; i<NUM_ROOMS; i++)
	{
		printf("ROOM NAME: %s\n", chosenrooms[i].name);

		int j;
		for (j=0; j<chosenrooms[i].numlinks; j++)
		{
			printf("CONNECTION %d: %s\n", (j + 1), chosenrooms[i].link[j]->name);
		}	
		
		printf("ROOM TYPE: %s\n\n", chosenrooms[i].type);
	}

	// generate room files

	// free any allocated memory
	FreeAtLast();
	return 0;
}
