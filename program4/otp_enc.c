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
 * any are not A-Z or ' ', then returns 1. Otherwise returns
 * 0.
 ***********************************************************/
int noBadChars(char* string)
{
	long i;
	// check every char in string
	for (i = 0; i < strlen(string); i++)
	{
		// if not A-Z, ' ' or '&',  return 0 (false)
		if (string[i] > 90 || (string[i] < 65 && string[i] != 32 && string[i] != 38))
		{
			return 0;
		}
	}
	return 1;
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

	// ensure key is as long as plaintext
	if (plainLen > keyLen)
	{
		stderror("Key is too short\n");
	}

	// check plaintext for bad characters
	if ((noBadChars(plaintext) * noBadChars(key)) != 1)
	{
		stderror("Input contains bad characters\n");		
	}

	

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

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0)
	{
		 stderror("CLIENT: ERROR opening socket\n");
	}

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
	{
		fprintf(stderr, "Could not contact otp_enc_d on port %d\n", portNumber);

		// close socket before exiting
		// close(socketFD);
		exit(2);
	}

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

	// receive message from server and compare to see if they can encode
	charsRead = recv(socketFD, buffer, sizeof(buffer), 0);
	if (charsRead < 0)
	{
		stderror("Error receiving data from server\n");
	}

	// if server is not able to encode, throw error
	if (strcmp(buffer, "serverEncode") != 0)
	{
		fprintf(stderr, "Could not contact otp_enc_d on port %d\n", portNumber);

		// close socket before exiting
		close(socketFD);
		exit(2);
	}	

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
	charsRead = recv(socketFD, ciphertext, sizeof(ciphertext), 0);
	if (charsRead < 0)
	{
		stderror("Error receiving data from server\n");
	}

	// re-append newline character before sending to stdout
	ciphertext[plainLen -1] = '\n';
	printf(ciphertext);

	// close the socket before exiting
	close(socketFD);
	return 0;
}

