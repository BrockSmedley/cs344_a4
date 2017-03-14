//otp_enc_d.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "otp_const.h"
#include "newmod.h"
#include "normchar.h"
#include "ciphr.h"

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[2 * NUM_BYTES];
	char plaintext[NUM_BYTES];
	char ciphertext[NUM_BYTES];
	char keytext[NUM_BYTES];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// make temp file to store port that otp_enc_d is using
	FILE *portfile = fopen("portenc", "w");
	fputs(argv[1], portfile);
	fclose(portfile);

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	/* Enable the socket to begin listening */
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Get the size of the address for the client that will connect
	sizeOfClientInfo = sizeof(clientAddress);

	while (1){
	  // Accept connections
	  establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	  int pid;
	  if ((pid = fork()) == -1){
	    close(establishedConnectionFD);
	    continue;
	  }
	  else if (pid > 0){
	    close(establishedConnectionFD);
	    continue;
	  }
	  else if (pid == 0){
	    // do stuff here
	    if (establishedConnectionFD < 0) error("ERROR on accept");

	    // clear out buffer
	    memset(buffer, '\0', NUM_BYTES);

	    /* loop to receive big transmissions */
	    int recvSize;
	    char chunk[255];
	    while (1){
	      memset(chunk, 0, 255); // clear out chunk
	      // read into chunk
	      if ((recvSize = recv(establishedConnectionFD, chunk, 255, 0)) <= 0)
		break;
	      else{
		//printf("SERVER: receiving chunk...\n");
		charsRead += recvSize;
		strcat(buffer, chunk);
		//printf("recvSize: %d\n", recvSize);
		// check for end of stream
		if (chunk[recvSize-1] == '@'){
		  //printf("Transmission complete\n");
		  break;
		}
	      }
	      // thanks http://www.binarytides.com/receive-full-data-with-recv-socket-function-in-c/
	    }
	    
	    // check for valid transmission
	    if (charsRead < 0) error("ERROR reading from socket");
	    //printf("ENC_SERVER: I received this from the client: \"%s\"\n", buffer);

	    /* Encrypt message */
	    // parse out plaintext
	    int i = 0;
	    while (buffer[i] != '&'){
	      plaintext[i] = buffer[i];
	      i++;
	    }
	    i++;	// skip past the & char
	    int j = 0;
	    while (buffer[i] != '@'){
	      keytext[j] = buffer[i];
	      i++;
	      j++;
	    }

	    //printf("[ENC_SERVER] plaintext: '%s'\n", plaintext);
	    //printf("[ENC_SERVER] keytext: '%s'\n", keytext);

	    // Create ciphertext
	    strcpy(ciphertext, cipher_str(plaintext, keytext));
	    strcat(ciphertext, "@");
	    // Send ciphertext back to client
	    charsRead = send(establishedConnectionFD, ciphertext, strlen(ciphertext), 0);
	    
	    if (charsRead < 0) error("ERROR writing to socket");
	    close(establishedConnectionFD); // Close the existing socket which is connected to the client
	    
	    break;
	  }

	  
	}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
