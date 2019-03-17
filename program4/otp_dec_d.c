/***********************************************************
 * Program: otp_dec_d.c
 * Author: Joel Huffman
 * Last updated: 3/17/2019
 * Description: Gets a port number and listens at that port
 * for a socket connection. Then receives and decrypts a 
 * ciphertext file with a keyfile through the connection. Finally
 * the decrypted file is sent back through the connection.
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
 * Function: decrypt(plaintext, key)
 * Accepts ciphertext and key strings. Decrypts the ciphertext 
 * using the key. Returns the now decrypted plaintext.
 ***********************************************************/
char* decrypt(char* ciphertext, char* key)
{
	// create plaintext to be filled with decrypted chars
	char* plaintext = malloc(sizeof(char) * strlen(ciphertext));

	int plainInt;
	int keyInt;
	int diffInt;
	int cipherInt;

	long i;
	// for each char in ciphertext
	for (i = 0; i < strlen(ciphertext); i++)
	{
		// convert char to ascii decimal value
		// subtract 65 to make A-Z be 0-25
		cipherInt = ciphertext[i] - 65;

		// ' ' will be represented as 26
		if (cipherInt == -33)
		{
			cipherInt = 26;
		}

		// do same thing with key chars
		keyInt = key[i] - 65;

		if (keyInt == -33)
		{
			keyInt = 26;
		}

		// get difference from cipher and key chars
		diffInt = cipherInt - keyInt;
		
		// keep diffInt positive
		if (diffInt < 0)
		{
			diffInt += 27;
		}

		// set plainInt to int representation of char for conversion
		plainInt = (diffInt % 27) + 65;

		// special case for ' ' again
		if (plainInt == 91)
		{
			plainInt = 32;
		}
		
		// put decrypted char into plaintext string
		plaintext[i] = plainInt;
	}
	return plaintext;
}

int main(int argc, char *argv[])
{
	int listenSocketFD, 
	    establishedConnectionFD, 
	    portNumber, 
	    charsRead,
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
		long ciphertextLen;

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
			if (strcmp(buffer, "clientDecode") == 0)
			{
				// let client know we are a server also trying to decode
				charsSent = send(establishedConnectionFD, "serverDecode", 12, 0); // Send success back
				if (charsSent < 0)
				{
					stderror("ERROR writing to socket\n");
				}

				// get the lengths of the plaintext and key to be sent over
				charsRead = recv(establishedConnectionFD, &ciphertextLen, sizeof(ciphertextLen), 0);
				if (charsRead < 0)
				{
					stderror("ERROR receiving ciphertext length\n");
				}

				charsRead = recv(establishedConnectionFD, &keyLen, sizeof(keyLen), 0);
				if (charsRead < 0)
				{
					stderror("ERROR receiving key length\n");
				}

				// ensure key is large enough for plaintext
				if (keyLen >= ciphertextLen)
				{
					char ciphertext[ciphertextLen];
					char key[keyLen];

					// get ciphertext across socket
					charsRead = recv(establishedConnectionFD, &ciphertext, sizeof(ciphertext), MSG_WAITALL);
					if (charsRead < 0)
					{
						error("ERROR receiving ciphertext\n");
					}

					// get key across socket
					charsRead = recv(establishedConnectionFD, &key, sizeof(key), MSG_WAITALL);
					if (charsRead < 0)
					{
						error("ERROR receiving key\n");
					}

					// decrypt ciphertext using key
					char* plaintext = decrypt(ciphertext, key);

					// send plaintext back across socket
					charsSent = send(establishedConnectionFD, plaintext, strlen(plaintext), 0);
					if (charsSent < 0)
					{
						error("ERROR writing to socket\n");
					}

					// close socket to client
					close(establishedConnectionFD); 
					exit(0);
				}
			}

			// otherwise let the client know this is a mismatch so it can disconnect
			else
			{
				charsSent = send(establishedConnectionFD, "ABORT!", 6, 0);
				if (charsSent < 0)
				{
					stderror("ERROR sending data from server\n");
				}				
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
