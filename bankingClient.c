#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "commons.h"

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
        struct hostent * server = gethostbyname(srvname);
	printf("Host name acquired.\n");
        if (server == NULL) {
            	printf("Error! No host named %s\n", srvname);
            	exit(0);
        }
//	printf("sockfd: %d\n", sockfd);	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portnum);
	printf("Attempting to connect...\n");
	int cnct = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    	if (cnct < 0){
        	error("Error! Could not connect.\n");
	}
	printf("Connected to server %s\n", srvname);
    	printf("Enter command:  ");
	char buffer[512];
	memset(&buffer,'0', 512);
	while(strcmp(buffer, "Shutdown") != 0) {
    		printf("Enter command:  ");
		memset(&buffer,'0', 512);
		fgets(buffer, 511, stdin);
		int n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) {
		     error("Error! Could not write to socket.\n");
		}
		bzero(buffer,256);
		n = read(sockfd, buffer, 255);
		if (n < 0) {
		     error("Error! Could not read from socket.\n");
		}
		printf("%s\n",buffer);
	}

	return 0;
}
