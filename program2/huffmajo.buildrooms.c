/***********************************************************
 * Program: huffmajo.buildrooms.c
 * Author: Joel Huffman
 * Last updated: 2/2/2019
 * Sources: 
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
char* roomnames[TOTAL_ROOMS] = {"RED", "ORANGE", "YELLOW", "GREEN", "BLUE",
				"INDIGO", "VIOLET", "BLACK", "WHITE", "GREY"};

char* roomtypes[ROOM_TYPES] = {"START_ROOM", "END_ROOM", "MID_ROOM"};

struct Room
{
	
}

void CreateRoomsDir ()
{
	// create directory with current PID
	char roomsdir[25];
	memset(roomsdir, '\0', 25);
	sprintf(roomsdir, "huffmajo.rooms.%d", getpid());
	mkdir(roomsdir, 0777);
}

void CreateRooms ()
{

}

int main ()
{
	// seed randomization
	srand(time(NULL));

	// create dir for room files
	CreateRoomsDir();

	// generate room files

	return 0;
}
