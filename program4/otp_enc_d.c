/***********************************************************
 * Program: otp_enc_d.c
 * Author: Joel Huffman
 * Last updated: 3/9/2019
 * Description: Gets a port number and listens at that port
 * for a socket connection. Then receives and encrypts a 
 * plainText file with a keyfile through the connection. Finally
 * the encrypted file is sent back through the connection.
 * Sources:
 * https://beej.us/guide/bgnet/html/multi/recvman.html
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

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
 * Function: encrypt(plaintext, key)
 * Accepts both plaintext and key strings. Encrypts the 
 * plaintext using the key. Returns the now encrypted plaintext.
 ***********************************************************/
char* encrypt(char* plaintext, char* key)
{
	// create ciphertext to be filled with encrypted chars
	char ciphertext[strlen(plaintext)];

	int plainInt;
	int keyInt;
	int sumInt;
	int cipherInt;

	int i;
	// for each char in plaintext
	for (i = 0; i < strlen[plaintext]; i++)
	{
		// get integer representation of each plaintext char
		// special case for space since it's not in sequence with A-Z
		if (plaintext[i] == ' ')
		{
			plainInt = 26;
		}

		// otherwise convert as usual
		else
		{
			plainInt = atoi(plaintext[i]) - 65;
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

		// add together converted chars
		sumInt = plainInt + keyInt;
		
		// keep sumInt under 27
		if (sumInt > 26)
		{
			sumInt -= 26;
		}

		// set cipherInt to int representation of char for conversion
		cipherInt = (sumInt % 26) + 65;

		// special case for ' ' again
		if (cipherInt == 0)
		{
			cipherInt = 32;
		}
		
		//convert cipherInt to corresponding char and place in ciphertext
		frpintf(ciphertext[i], "%c", cipherInt);
	}

}

int main(int argc, char *argv[])
{
	int listenSocketFD, 
	    establishedConnectionFD, 
	    portNumber, 
	    charsRead;
	    charsSent;
	socklen_t sizeOfClientInfo;
	int bufferSize = 70000;
	char buffer[bufferSize];
	struct sockaddr_in serverAddress, 
			   clientAddress;

	// check if valid number of arguments
	if (argc < 2) 
	{ 
		fprintf(stderr,"USAGE: %s port\n", argv[1]); 
		exit(1); 
	} 

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket

	// error if socket fails
	if (listenSocketFD < 0) 
	{
		stderror("ERROR opening socket\n");
	}

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		stderror("ERROR on binding\n");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

	while (1)
	{
		pid_t pid;
		long keyLen;
		long plaintextLen;

		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) 
		{
			// print error, but don't exit
			fprintf(stderr, "ERROR on accept\n");
		}

		// fork off child process for connection
		pid = fork();

		// error with fork
		if (pid == -1)
		{
			stderror("Fork error\n");
		}

		// child process
		else if (pid == 0)
		{
			// Get the message from the client and display it
			memset(buffer, '\0', sizeof(buffer));
			charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); // Read the client's message from the socket
			if (charsRead < 0) 
			{
				stderror("ERROR reading from socket\n");
			}
			printf("Server received this from client: \"%s\"\n", buffer);

			// if both client and server are encoding continue	
			if (cmpstr(buffer, "clientEncode") == 0)
			{
				// let client know we are a server also trying to encode
				charsSent = send(establishedConnectionFD, "serverEncode", 12, 0); // Send success back
				if (charsSent < 0)
				{
					error("ERROR writing to socket\n");
				}

				// get the lengths of the plaintext and key to be sent over
				charsRead = recv(establishedConnectionFD, &plaintextLen, sizeof(plaintextLen), 0);
				if (charsRead < 0)
				{
					error("ERROR receiving plaintext length\n");
				}

				charsRead = recv(establishedConnectionFD, &keyLen, sizeof(keyLen), 0);
				if (charsRead < 0)
				{
					error("ERROR receiving key length\n");
				}

				// ensure key is large enough for plaintext
				if (keyLen >= plaintextLen)
				{
					char plaintext[plaintextLen];
					char key[keyLen];

					// get plaintext across socket
					charsRead = recv(establishedConnectionFD, &plaintext, sizeof(plaintext), MSG_WAITALL);
					if (charsRead < 0)
					{
						error("ERROR receiving plaintext\n");
					}

					// get key across socket
					charsRead = recv(establishedConnectionFD, &key, sizeof(key), MSG_WAITALL);
					if (charsRead < 0)
					{
						error("ERROR receiving key\n");
					}

					//########### This might go in opt_enc
					// check for bad chars in either plaintext or key
					if (noBadChars(plaintext) * nobadChars(key))
					{
						stderror("input contains bad characters\n");
					}
	
					// encrypt plaintext using key
					char* ciphertext = encrypt(plaintext, key);

					// send ciphertext back across socket
					charsSent = send(establishedConnectionFD, ciphertext, strlen(ciphertext), 0);
					if (charsSent < 0)
					{
						error("ERROR writing to socket\n");
					}

					// close socket to client
					close(establishedConnectionFD); 
					exit(0);
				}
			}

			// otherwise don't keep sending stuff
			else
			{

			}
				
		}

		// parent process
		else 
		{

			// close socket to client
			close(establishedConnectionFD); 			
		}

	}
	// done waiting for responses across socket
	close(listenSocketFD);

	return 0; 
}

