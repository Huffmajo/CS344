/***********************************************************
 * Program: otp_enc.c
 * Author: Joel Huffman
 * Last updated: 3/17/2019
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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/***********************************************************
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
 * any are not A-Z or ' ', then returns -1. Otherwise returns
 * the index of first bad character.
 ***********************************************************/
int noBadChars(char* string)
{
	long i;
	// check every char in string
	for (i = 0; i < strlen(string); i++)
	{
		// if not A-Z, ' ' or '&',  return i (false)
		if (string[i] > 90 || (string[i] < 65 && string[i] != 32 && string[i] != 38))
		{
			return i;
		}
	}
	return -1;
}

int main(int argc, char *argv[])
{
	int socketFD, 
	    portNumber, 
	    charsWritten, 
	    charsRead,
	    charsSent;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	int bufferSize = 150000;
	char buffer[bufferSize];
	char plaintext[bufferSize];
	char key[bufferSize];
	char ciphertext[bufferSize];
	long plainLen;
	long keyLen;
//	int plainFile;
//	int keyFile;
    
	// check if valid number of arguments
	if (argc < 4) 
	{ 
		fprintf(stderr,"Wrong number of arguments\n"); 
		exit(1); 
	} 

	FILE* plainFile = fopen(argv[1], "r");
	fgets(plaintext, bufferSize , plainFile);
	plainLen = strlen(plaintext);
	plaintext[plainLen - 1] = '&';
	fclose(plainFile);

	FILE* keyFile = fopen(argv[2], "r");
	fgets(key, bufferSize , keyFile);
	keyLen = strlen(key);
	key[keyLen - 1] = '&';
	fclose(plainFile);

//	printf("\ntextfile: %s\n", plaintext);
//	printf("key: %s\n\n", key);

	int pBadChar;
	int kBadChar;

	// check plaintext for bad characters
	if (pBadChar = noBadChars(plaintext) != -1)
	{
		printf("Plaintext: bad char %c at index %d\n", plaintext[pBadChar], pBadChar);
		stderror("Plaintext contains bad characters\n");		
	}

	// check key for bad characters
	if (kBadChar = noBadChars(key) != -1)
	{
		printf("Key: bad char %c at index %d\n", key[kBadChar], kBadChar);
		stderror("Key contains bad characters\n");
	}
/*
	// check files for bad characters
	if (noBadChars(plaintext) * noBadChars(key) == 0)
	{
		stderror("Input contains bad characters\n");
	}	

	// check that plaintext is readable
	if (plainFile = open(argv[1], O_RDONLY) < 0)
	{
		stderror("plaintext file unreadable\n");
	}

	// check that keyfile is also readable
	if (keyFile = open(argv[2], O_RDONLY) < 0)
	{
		stderror("key file unreadable\n");
	}

	// read in plaintext file 
	read(plainFile, plaintext, bufferSize);

	// read in key file
	read(keyFile, key, bufferSize);

	// get plaintext and key lengths
	plainLen = strlen(plaintext);
	keyLen = strlen(key);

	// replace ending newline character from plaintext and key with '\0'
	plaintext[plainLen - 1] = '\0';
	key[keyLen - 1] = '\0';

	// key can't be shorter than plaintext
	if (plainLen > keyLen)
	{
		printf("keyLen: %d\n", keyLen);
		printf("plainLen: %d\n", plainLen);
		stderror("Key is too short\n");	
	}

	// close files
	// close(plainFile);
	// close(keyFile);

	printf("\ntextfile: %s\n", plaintext);
	printf("key: %s\n\n", key);

	// check files for bad characters
	if (noBadChars(plaintext) * noBadChars(key) == 0)
	{
		stderror("Input contains bad characters\n");
	}
*/

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Use localhost as server and client are on same machine
	if (serverHostInfo == NULL) 
	{ 
		fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
		exit(1); 
	}

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

//	printf("1\n");

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0)
	{
		 stderror("CLIENT: ERROR opening socket\n");
	}

//	printf("2\n");	

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
	{
		fprintf(stderr, "Could not contact otp_enc_d on port %d\n", portNumber);

		// close socket before exiting
		// close(socketFD);
		exit(2);
	}

//	printf("3\n");

	// send server message we're the client and we want to encode
	charsSent = send(socketFD, "clientEncode", 12, 0);
	if (charsSent < 0) 
	{
		stderror("CLIENT: ERROR writing to socket\n");
	}
	if (charsSent < 12) 
	{
		printf("Not all data sent to the server: clientEncode\n");
	}

//	printf("4\n");

	// receive message from server and compare to see if they can encode
	charsRead = recv(socketFD, buffer, sizeof(buffer), 0);
	if (charsRead < 0)
	{
		stderror("Error receiving data from server\n");
	}

//	printf("5\n");

	// if server is not able to encode, throw error
	if (strcmp(buffer, "serverEncode") != 0)
	{
		fprintf(stderr, "Could not contact otp_enc_d on port %d\n", portNumber);

		// close socket before exiting
		close(socketFD);
		exit(2);
	}	

//	printf("6\n");

	// send length of plaintext
	charsSent = send(socketFD, &plainLen, sizeof(plainLen), 0);
	if (charsSent < 0) 
	{
		stderror("CLIENT: ERROR writing to socket\n");
	}

	if (charsSent < sizeof(plainLen)) 
	{
		printf("Not all data sent to the server\n");
	}

//	printf("7\n");

	// send length of key
	charsSent = send(socketFD, &keyLen, sizeof(keyLen), 0);
	if (charsSent < 0) 
	{
		stderror("CLIENT: ERROR writing to socket\n");
	}

	if (charsSent < sizeof(keyLen)) 
	{
		printf("Not all data sent to the server\n");
	}

	// send plaintext
	charsSent = send(socketFD, plaintext, strlen(plaintext), 0);
	if (charsSent < 0) 
	{
		stderror("CLIENT: ERROR writing to socket\n");
	}

	if (charsSent < strlen(plaintext)) 
	{
		printf("Not all data sent to the server\n");
	}

	// send key
	charsSent = send(socketFD, key, strlen(key), 0);
	if (charsSent < 0) 
	{
		stderror("CLIENT: ERROR writing to socket\n");
	}

	if (charsSent < strlen(key)) 
	{
		printf("Not all data sent to the server\n");
	}

	// receive encoded ciphertext
	charsRead = recv(socketFD, ciphertext, sizeof(ciphertext), MSG_WAITALL);
	if (charsRead < 0)
	{
		stderror("Error receiving data from server\n");
	}

	// re-append newline character before sending to stdout
	fprintf(stdout, "%s\n", ciphertext);

	// close the socket before exiting
	close(socketFD);
	return 0;
}

