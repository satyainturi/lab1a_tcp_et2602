//#define _WINDOWS

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef _WINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
#include "proc.h"

// Protocals supported in server side.
#define PROTOCALS "TEXT TCP 1.0\nTEXT TCP 1.2\n"



void help();
void process(int connfd);
void get_rand_command(struct command* cmd);
void get_listen_address(char* arg, char* address);
int  get_listen_port(char* arg);
void get_user_input_command(struct command* cmd);

int main(int argc, char** argv) {
	printf("Server is starting...\n");

	// server MUST be started with listen IP and port.
	if (argc < 2) {
		help();
	}
	
	char address[40];
	get_listen_address(argv[1], address);
	int port = get_listen_port(argv[1]);

#ifdef _WINDOWS
	// startup winsock.
	WSADATA wsa;
	WSAStartup(202, &wsa);
#endif
		
	// socket create and verification 
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("Socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
	}
	
	int seed = sockfd;
	srand((unsigned int)seed);

	struct sockaddr_in servaddr; 
	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(address); //htonl(INADDR_ANY); //TODO-localhost
	servaddr.sin_port = htons(port); 
	
	// Binding newly created socket to given IP
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("Socket bind failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully binded..\n"); 
	}

	while (1) {
		// Now server is ready to listen
		if ((listen(sockfd, 5)) != 0) { 
			printf("Listen failed...\n"); 
			exit(0); 
		} 
		else {
			printf("\nServer is listening on %s:%d\n", address, port); 
		}
		
		struct sockaddr_in client; 
		int len = sizeof(client); 
		
		// Accept the data packet from client
		int connfd = accept(sockfd, (struct sockaddr*)&client, &len); 
		if (connfd < 0) { 
			printf("server acccept failed...\n"); 
			exit(0);
		} 
		else {
			printf("server acccept the client...\n"); 
		}
		
		// Function for messaging between client and server 
		process(connfd); 

		//  close the client socket 
		close(connfd);
	}

	// After chatting close the socket 
	close(sockfd); 
  
	return 0;
}

void help() {
	printf("\nYou have to start server with listen address.\n");
	printf(" ./server.exe localhost:8080 \n\n");
	exit(0);
}

void get_listen_address(char* arg, char* address) {
	if (strlen(arg) >= 20) {
		help();
	}

	strcpy(address, arg);
	char* ptr = strchr(address, ':');\
	if (ptr == 0) {
		help();
	}
	*ptr = 0;
}

int get_listen_port(char* arg) {
	char* ptr = strchr(arg, ':');
	return atoi(ptr + 1);
}

int check_protocal_version(int connfd) {
	char protocals[] = PROTOCALS;
	int result = send(connfd, protocals, sizeof(protocals), 0);
	if (result < 0) {
		printf("Socket error\n");
		exit(0);
	}

	char buf[1024];
	memset(buf, 0, sizeof(buf));

	result = recv(connfd, buf, sizeof(buf), 0);
	if (result < 0) {
		printf("Socket error\n");
		exit(0);
	}

	if (strcmp(buf, "OK") != 0) {
		//The client don't support protocal.
		return -1;
	}

	return 0;
}

void get_user_input_command(struct command* cmd) {
	while (1) {
		printf("Input a command (ex: add 5 6): ");
		
		char buf[260];
		gets(buf);

		int count = 0;
		if (buf[0] == 'f') {
			count = sscanf(buf, "%s %lg %lg", cmd->str, &cmd->fval1, &cmd->fval2);
		}
		else {
			count = sscanf(buf, "%s %d %d", cmd->str, &cmd->val1, &cmd->val2);
		}
		if (count == 3) {
			return;
		}
	}
}

void process(int connfd) {
	// check client's protocal version.
	// if the client accepts the protocol, it should respond with "OK".
	if (check_protocal_version(connfd) != 0) {
		printf("The client don't support protocal.\n");
		return;
	}

	// create command to be sent to the client.
	struct command cmd;
	create_command(&cmd); //get_user_input_command(&cmd);
	

	char msg[400];
	get_command_string(&cmd, msg);
	printf("Send message %s", msg);

	int result = send(connfd, msg, strlen(msg) + 1, 0);
	if (result < 0) {
		printf("Socket error.\n");
		return;
	}

	memset(msg, 0, sizeof(msg));
	result = recv(connfd, msg, sizeof(msg), 0);
	if (result < 0) {
		printf("Socket error.\n");
		return;
	}
	printf("Received '%s' from the client\n", msg);

	double server_val = calculate(&cmd);
	//printf("Server value is '%.8g'\n", server_val);

	double client_val = 0.0;
	sscanf(msg, "%lg", &client_val);
	//printf("Client value is '%.8g'\n", client_val);

	char* response = "OK";
	if (server_val != client_val) {
		//There is a issue in double operation. 
		//Double operation result is sligty different every computers.
		if (cmd.str[0] == 'f') {
			if (fabs(server_val - client_val) > 1) {
				response = "ERROR";
			}
		}
	}
	printf("Compare result is %s\n", response);
	
	result = send(connfd, response, strlen(response) + 1, 0);
	if (result < 0) {
		printf("Socket error.\n");
		return;
	}
}