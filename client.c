//#define _WINDOWS

#include <stdio.h>
#include <unistd.h>

#ifdef _WINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#define PROTOCAL "TEXT TCP 1.0"

struct command {
	char cmd[10];
	double fval1, fval2;
	int val1, val2;
};

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
void calc();
int  check_protocols(char*);
void fail();
void get_listen_address(char* arg, char* address);
void help();
int  get_listen_port(char* arg);

int main(int argc, char** argv) {
	
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
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
	}
	
	struct sockaddr_in servaddr; 
	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(address); 
	servaddr.sin_port = htons(port); 
	int ret	 = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	printf("connected to server\n", ret);
	
	//receive the string from server and test the string
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	int len = recv(sockfd, buf, sizeof(buf), 0);	
	if (len < 0) {
		fail();
	}

	if (check_protocols(buf) == 0) {
		fail();
	}
	len = send(sockfd, "OK", 2, 0);
	if (len < 0) {
		fail();
	}	

	//receive cal string
	char calc_string[50];
	memset(calc_string, 0, sizeof(calc_string));

	len = recv(sockfd, calc_string, sizeof(calc_string), 0);
	if (len < 0) {
		fail();
	}
	printf("server said (%s)\n", calc_string);

	char result[20];
	memset(result, 0, sizeof(result));
	calc(calc_string, result); 
	len = send(sockfd, result, strlen(result), 0);
	printf("send result(%s) to server.\n", result);
	if (len < 0) {
		fail();
	}

	char response[50];
	memset(response, 0, sizeof(response));
	len = recv(sockfd, response, sizeof(response), 0);
	if (len < 0) {
		fail();
	}
	printf("server reply is (%s)\n", response);
	// After chatting close the socket 
	close(sockfd); 
  
	return 0;
}

void fail() {
	printf("connection is failed\n");
	exit(0);
}

void get_listen_address(char* arg, char* address) {
	if (strlen(arg) >= 20) {
		help();
	}

	strcpy(address, arg);
	char* ptr = strchr(address, ':');
	if (ptr == 0) {
		help();
	}

	*ptr = 0;
}

void help() {
	printf("\nYou have to start client with server address.\n");
	printf(" ./client.exe 192.168.1.2:8080 \n\n");
	exit(0);
}

int get_listen_port(char* arg) {
	char* ptr = strchr(arg, ':');
	return atoi(ptr + 1);
}

int check_protocols(char* string) {
	char *ptr = string;
	char temp[20];
	
	while(*ptr != 0) {
		ptr = strchr(string, '\n');
		if (ptr == 0) {
			break;
		}
		
		memset(temp, 0, sizeof(temp));
		strncpy(temp, string, ptr - string);
		if (strcmp(temp, PROTOCAL) == 0) {
			return 1;
		}
		string = ptr + 1;
	}
	return 0;
}

struct command split_calc_string(char* calc_string) {
	struct command cs;
	if (calc_string[0] == 'f') {
		sscanf(calc_string, "%s %lg %lg", cs.cmd, &cs.fval1, &cs.fval2);
	}
	else {
		sscanf(calc_string, "%s %d %d", cs.cmd, &cs.val1, &cs.val2);
	}
	return cs;
}

double get_int_result(char* cmd, int value1, int value2) {
	if (strcmp(cmd, "add") == 0) {
		return (double)(value1 + value2);
	}
	else if (strcmp(cmd, "mul") == 0) {
		return (double)(value1 * value2);
	}
	else if (strcmp(cmd, "sub") == 0) {
		return (double)(value1 - value2);
	}
	else if (strcmp(cmd, "div") == 0) {
		return (double)value1 / (double)value2;
	}
	return 0.0;
}

double get_double_result(char* cmd, double value1, double value2) {
	if (strcmp(cmd, "fadd") == 0) {
		return value1 + value2;
	}
	else if (strcmp(cmd, "fmul") == 0) {
		return value1 * value2;
	}
	else if (strcmp(cmd, "fsub") == 0) {
		return value1 - value2;
	}
	else if (strcmp(cmd, "fdiv") == 0) {
		return value1 / value2;
	}
	return 0.0;
}

void calc(char* calc_string, char* result) {
	printf("calc...\n");
	
	struct command cs = split_calc_string(calc_string);	

	if (cs.cmd[0] == 'f') {
		double double_result = get_double_result(cs.cmd, cs.fval1, cs.fval2);
		sprintf(result, "%.8g", double_result);
	}
	else {
		double int_result = get_int_result(cs.cmd, cs.val1, cs.val2);
		sprintf(result, "%.8g", int_result);
	}
	printf("calc completed!\n");

}