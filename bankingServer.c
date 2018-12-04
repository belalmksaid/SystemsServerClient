
#include "commons.h"
#include "account.h"
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

volatile bool INTERRUPTED = false;
volatile bool PASTACCEPT = false;
volatile linked_list threads;

void SIGINT_HANDLER(int d) {
    INTERRUPTED = true;
    if(!PASTACCEPT)
        exit(0);
}

void process_socket() {

}

int main(int argc, char** argv) {
    signal(SIGINT, SIGINT_HANDLER);
    threads.head = NULL;

    int port = -1;
    
    if(argc != 2) {
        error("Incorrect number of arguments supplied. Expected one: port number.");
    }
    else {
        port = atoi(argv[1]);
        if(port <= 0) {
            error("Incorrect argument for port number.");
        }
    }

    int sockfd, newsockfd;
    char buffer[256];
    struct sockaddr_in serv_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    memset((char *) &serv_addr, sizeof(serv_addr), 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd, 128);

    while(!INTERRUPTED) {
        PASTACCEPT = false;
        struct sockaddr_in clientaddress;
        int clientsize = sizeof(clientaddress);
        int newsock = accept(sockfd, (struct sockaddr*) &clientaddress, &clientsize);
        PASTACCEPT = true;
        if(newsock < 0) {
            error("accept() failed");
        }
        thread_node* node = create_thread_node(&threads);
        node->newsocket_fd = newsock;
        int error = pthread_create(&(node->t_id), NULL, process_socket, (void *)node);
    } 
    
    //interruption handling
    {

    }
    return 0;
}