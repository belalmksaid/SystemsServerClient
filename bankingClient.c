#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "commons.h"

//struct to serve as input and output to read_srv function (for multithreading).
typedef struct { 
	char * buffer;
	int sockfd;
} thread_pointer;

//multithreading function
void * read_srvr(void * args) {
	thread_pointer * tp = (thread_pointer*)args;
	char * buffer = tp->buffer;
	int sockfd = tp->sockfd;
	int n = read(sockfd, buffer, 255);
	if (n < 0) {
	     error("Error! Could not read from socket.\n");
	}
	else {
		printf("%s\n",buffer);
	}
//FREE my n****s
	return NULL;
}

int main(int argc, char ** argv) {
	if(argc != 3) {
		printf("Error! Please include name of server to connect to and port number.\n");
		exit(1);
	}
	char * srvname = argv[1];
//	printf("Server name: %s", srvname);
	int portnum = atoi(argv[2]);
	//printf("portnum: %d", portnum);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            	error("Error opening socket\n");
		exit(1);	
	}
	printf("Host name acquired.\n");
//	printf("sockfd: %d\n", sockfd);	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portnum);
	//serv_addr.sin_addr.s_addr = inet_addr(srvname);
	struct hostent * server;
	server = gethostbyname(srvname);
        if (server == NULL) {
            	error("Error! Invalid hostname.");
        }
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	printf("Attempting to connect...\n");
	int cnct = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    	while (cnct < 0){
        	printf("Error! Could not connect to %s. Trying again in 3 seconds...\n", srvname);
		sleep(3);
		cnct = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	}
	printf("Connected to server %s\n", srvname);
    	printf("Enter command:  ");
	char buffer[512];
	memset(&buffer,'0', 512);
	bool quit = false;
	while(strcmp(buffer, SHUTDOWNMESSAGE) != 0) {
    		printf("Enter command:  ");
		memset(&buffer,'0', 512);
		fgets(buffer, 511, stdin);
		if(strcmp(buffer, "quit\n") == 0) {
			quit = true;
		}
		else {
			printf("%s", buffer);
		}
		int n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) {
		     error("Error! Could not write to socket.\n");
		}
		bzero(buffer,256);
		thread_pointer * tp = (thread_pointer*)malloc(sizeof(thread_pointer));
		tp->buffer = buffer;
		tp->sockfd = sockfd;
		pthread_t tid;
	 	int err = pthread_create(&tid, NULL, read_srvr, (void *)tp);
		if(err != 0){
			error("Error! Could not create thread!");
		}
		pthread_join(tid, NULL);
	//	read_srvr((void*)tp);
		if(quit == true) {
			break;
		}
		sleep(2);
	}
	printf("Server disconnected.\n");

	return 0;
}
