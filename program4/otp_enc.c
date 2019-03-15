/***********************************************************
 * Program: otp_enc.c
 * Author: Joel Huffman
 * Last updated: 3/9/2019
 * Description: Accepts plainText file, keyfile and port number.
 * Sends plainText and keyfile through connected port for 
 * encryption. Then receives encrypted text back and outputs it
 * to stdout.
 * Sources: 
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

}/***********************************************************
 * Function: stderror(err)
 * Accepts a string. Sends that string to standard error and
 * exits with value 1.
 ***********************************************************/
void stderror(const char* string) 
{ 
	fprintf(stderr, string); 
	exit(1); 
}

/***********************************************************
 * Function: noBadChars(string)
 * Accepts a string. Checks all characters in that string, if
 * any are not A-Z or ' ', then returns 0. Otherwise returns
 * 1.
 ***********************************************************/
int noBadChars(char* string)
{
	long i;
	// check every char in string
	for (i = 0; i < strlen(string); i++)
	{
		// if not A-Z or ' ', return 0 (false)
		if (string[i] != 32 || (string[i] < 65 && string[i] > 90))
		{
			return 0;
		}
	}
	return 1;
}

/***********************************************************
 * Function: decrypt(ciphertext, key)
 * Accepts both ciphertext and key strings. Decrypts the 
 * ciphertext using the key. Returns the now decrypted plaintext.
 ***********************************************************/
char* decrypt(char* ciphertext, char* key)
{
	// create plaintext to be filled with decrypted chars
	char plaintext[strlen(ciphertext)];

	int plainInt;
	int keyInt;
	int diffInt;
	int cipherInt;

	int i;
	// for each char in ciphertext
	for (i = 0; i < strlen[ciphertext]; i++)
	{
		// get integer representation of each plaintext char
		// special case for space since it's not in sequence with A-Z
		if (ciphertext[i] == ' ')
		{
			cipherInt = 26;
		}

		// otherwise convert as usual
		else
		{
			cipherInt = atoi(ciphertext[i]) - 65;
		}

		// do same thing with key chars
		if (key[i] == ' ')
		{
			keyInt = 26;
		}
		else
		{
			keyInt = atoi(key[i]) - 65;
		}

		// subtract key chars from cipher chars
		diffInt = cipherInt - keyInt;
		
		// keep diffInt positive
		if (diffInt < 0)
		{
			diffInt += 27;
		}

		// set plaintextInt to int representation of char for conversion
		plainInt = (diffInt % 27) + 65;

		// special case for ' ' again
		if (plainInt == 26)
		{
			plainInt = 32;
		}
		
		//convert plaintextInt to corresponding char and place in plaintext
		frpintf(plaintext[i], "%c", plainInt);
	}

} 

int main(int argc, char *argv[])
{
	int socketFD, 
	    portNumber, 
	    charsWritten, 
	    charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	int bufferSize = 70000;
	char buffer[bufferSize];
    
	// Check if valid number of arguments
	if (argc < 3) 
	{ 
		fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
		exit(0); 
	} 

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Get input message from user
	printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}

