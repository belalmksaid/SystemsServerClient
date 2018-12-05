
#include "commons.h"
#include "account.h"
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <fcntl.h>

volatile bool INTERRUPTED = false;
volatile bool PASTACCEPT = false;
volatile linked_list threads;
volatile bank mainbank;

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

void flag_print_all(int sig) {
    if(INTERRUPTED) return;
    pthread_mutex_lock(&print_mutex);
    printing = true;
    if(mainbank.size > 0)
        pthread_barrier_init(&print_barrier, NULL, threads.size);
    else {
        printing = false;
        printf("There are no accounts in the bank to print\n");
    }
    pthread_mutex_unlock(&print_mutex);
}

void print_accounts() {
    if(printing) {
        pthread_barrier_wait(&print_barrier);
        pthread_mutex_lock(&print_mutex);
        if(printing) {
            pthread_mutex_lock(&(mainbank.bank_lock));
            int i = 0;
            printf("Diagnostic Account Print\n");
            for(i = 0; i < mainbank.size; i++) {
                printf("Information for account: %s\n", mainbank.accounts[i].name);
                printf("Balance: $%lf\n", mainbank.accounts[i].balance);
                printf("Session flag: ");
                if(mainbank.accounts[i].session == IN_SESSION) {
                    printf("In Session\n\n");
                }
                else {
                    printf("Not in Session\n\n");
                }
            }
            pthread_mutex_unlock(&(mainbank.bank_lock));
            printing = false;
        }
        pthread_mutex_unlock(&print_mutex);
    }
}

void parse_command(char* buffer, int n, serve_session* session) {
    if(strstr(buffer, CREATE) != NULL) {
        if(session->acc == NULL) {
            if(n <= (CREATE_LEN + 1)) {
                write(session->node->newsocket_fd, INVALIDCOMMAND, INVALIDCOMMAND_LEN);
            }
            else {
                if(add_account(&mainbank, buffer + CREATE_LEN + 1, 0.0, NOT_IN_SESSION) == 0) {
                    write(session->node->newsocket_fd, ACCOUNTEXISTS, ACCOUNTEXISTS_LEN);
                }
                else {
                    write(session->node->newsocket_fd, ACCOUNTSUCCESS, ACCOUNTSUCCESS_LEN);
                }
            }
        }
        else {
            write(session->node->newsocket_fd, CANNOTCREATEACCT, CANNOTCREATEACCT_LEN);
        }
    }
}

void process_socket(void* nd) {
    thread_node * node = (thread_node*) nd;
    int n;
    serve_session session;
    session.acc = NULL;
    session.node = node;
    int flags = fcntl(node->newsocket_fd, F_GETFL, 0);
    fcntl(node->newsocket_fd, F_SETFL, flags | O_NONBLOCK);
    while(!(node->die)) {
        char buffer[1024];
        n = read(node->newsocket_fd, buffer, 1023);
        if(n > 0) {
            parse_command(buffer, n, &session);
        }
        print_accounts();
    }
    {
        write(node->newsocket_fd, SHUTDOWNMESSAGE, SHUTDOWNMESSAGE_LEN);
        shutdown(node->newsocket_fd, SHUT_RDWR);
    }
    print_accounts();
    delete_node(node);
}

int main(int argc, char** argv) {
    signal(SIGINT, SIGINT_HANDLER);
    if(signal(SIGALRM, flag_print_all) == SIG_ERR) {
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
    mainbank = create_bank();

    int port = -1;
    if(argc != 2) {
        error("ERROR incorrect number of arguments supplied. Expected one: port number.");
    }
    else {
        port = atoi(argv[1]);
        if(port <= 0) {
            error("ERROR incorrect argument for port number.");
        }
    }

    int sockfd, newsockfd;
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