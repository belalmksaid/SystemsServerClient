
#include "commons.h"
#include "account.h"
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

volatile bool INTERRUPTED = false;
volatile bool PASTACCEPT = false;
volatile linked_list threads;

volatile bool printing = false;
pthread_mutex_t print_mutex;
pthread_barrier_t print_barrier;

void SIGINT_HANDLER(int d) {
    INTERRUPTED = true;
    if(!PASTACCEPT) {
        kill_all();
        exit(0);
    }
}

void kill_all() {
    thread_node* trav = threads.head, *temp;
    pthread_t* ids = (pthread_t*)malloc(threads.size * sizeof(pthread_t));
    int i = 0, j = 0;
    while(trav != NULL) {
        ids[i++] = trav->t_id;
        temp = trav->next;
        trav->die = true;
        trav = temp;
    }
    for(j = 0; j < i; j++) {
        pthread_join(ids[j], NULL);
    }
    free(ids);
}

void print_all(int sig) {
    
}

void process_socket(void* nd) {
    thread_node * node = (thread_node*) nd;
    while(!(node->die)) {

    }
    {
        write(node->newsocket_fd, SHUTDOWNMESSAGE, SHUTDOWNMESSAGE_LEN);
        shutdown(node->newsocket_fd, SHUT_RDWR);
    }
    delete_node(node);
}

int main(int argc, char** argv) {
    signal(SIGINT, SIGINT_HANDLER);
    if(signal(SIGALRM, print_all) == SIG_ERR) {
        error("Could not signal alarm.");
    }

    struct itimerval it_val;
    it_val.it_value.tv_sec = PRINTINTERVAL / 1000;
    it_val.it_value.tv_usec = (PRINTINTERVAL * 1000) % 1000000;   
    it_val.it_interval = it_val.it_value; 
    if(setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        error("Could not set timer.");
    }

    threads.head = NULL;
    threads.size = 0;

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
        kill_all();
    }
    return 0;
}