// otp_enc.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include "otp_const.h"

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[SIZE_BYTES];
	char plaintext[SIZE_BYTES];
	char keytext[SIZE_BYTES];
	char ciphertext[SIZE_BYTES];

    
	if (argc < 4) { fprintf(stderr,"USAGE: plaintext key port\n"); exit(0); } // Check usage & args

	// make sure we're using an approved port
	FILE *portdecFD = fopen("portdec", "r");
	if (portdecFD){
	  char decport[7];
	  fgets(decport, 7, portdecFD);
	  if (strcmp(decport, argv[3]) == 0){
	    fprintf(stderr, "You may not connect to otp_dec_d with %s\n", argv[0]);
	    exit(1);
	  }
	}
	
	/* read input from files */
	
	// read plaintext
	int fd_pt = open(argv[1], O_RDONLY);
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	ssize_t bytesRead = read(fd_pt, buffer, NUM_BYTES);
	// strip newline	
	strtok(buffer, "\n");
	close(fd_pt);
	// copy buffer into plaintext var
	strcpy(plaintext, buffer);

	// read key
	int fd_key = open(argv[2], O_RDONLY);
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	bytesRead = read(fd_key, buffer, NUM_BYTES);
	// strip newline
	strtok(buffer, "\n");
	close(fd_key);
	// copy buffer into keytext var
	strcpy(keytext, buffer);

	// verify length requirements
	if (strlen(keytext) < strlen(plaintext)){
	  fprintf(stderr, "Key must be at least as long as input\n");
	  exit(0);
	}

	/* verify valid characters */
	// for plaintext
	int i = 0;
	while (plaintext[i]){
	  char c = plaintext[i];
	  if ((c < 65 && c != 32) || (c > 90 && c < 97) || c > 122){ // alphabetic bounds
	    fprintf(stderr, "You have one or more invalid characters in your input file\n");
	    exit(1);
	  }
	  i++;
	}

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	
	// Send message to server
	// format: plaintext&key@
	// @ is our end of stream delimiter
	strcpy(buffer, plaintext);
	strcat(buffer, "&");
	strcat(buffer, keytext);
	strcat(buffer, "@");
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server; loop for big transmissions
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	memset(ciphertext, '\0', sizeof(ciphertext));

	int recvSize = 0;
	charsRead = 0;
	char chunk[255];
	while(1){
	  memset(chunk, 0, 255);

	  // Read data from the socket, leaving \0 at end
	  if ((recvSize = recv(socketFD, chunk, 255, 0)) <= 0){
	    break;
	  }
	  else{
	    //printf("CLIENT: receiving chunk...\n");
	    charsRead += recvSize;
	    strcat(buffer, chunk);
	    // check for end of stream
	    if (chunk[recvSize-1] == '@'){
	      //printf("Transmission complete\n");

	      break;
	    }
	  }
	}
	
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	strncpy(ciphertext, buffer, charsRead-1); // cut off EOS char
	printf("%s\n", ciphertext);

	close(socketFD); // Close the socket
	return 0;
}
