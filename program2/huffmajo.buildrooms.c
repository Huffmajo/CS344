#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>

void CreateRoomsDir ()
{
	// create directory with current PID
	char roomsdir[25];
	memset(roomsdir, '\0', 25);
	sprintf(roomsdir, "huffmajo.rooms.%d", getpid());
	mkdir(roomsdir, 0777);

	// print statement to check PID
	printf("Created new directory: %s\n", roomsdir);
}



int main ()
{
	
	CreateRoomsDir();

	

	return 0;
}
