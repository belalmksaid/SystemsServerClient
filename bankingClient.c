#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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
        if (server == NULL) {
            	printf("Error! No host named %s\n", srvname);
            	exit(0);
        }
//	printf("sockfd: %d\n", sockfd);	
	return 0;
}
