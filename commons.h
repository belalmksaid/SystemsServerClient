#ifndef COMMONS
#define COMMONS

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define bool char
#define true 1
#define false 0
#define PRINTINTERVAL 15000

///// Commands
#define CREATE "create"
#define CREATE_LEN 6

#define SERVE "serve"
#define SERVE_LEN 5

#define QUERY "query"
#define QUERY_LEN 5

#define END "end"
#define END_LEN 3

#define QUIT "quit"
#define QUIT_LEN 4

#define WITHDRAW "withdraw"
#define WITHDRAW_LEN 8

#define DEPOSIT "deposit"
#define DEPOSIT_LEN 7

///// Errors
#define SHUTDOWNMESSAGE "SHUTDOWN"
#define SHUTDOWNMESSAGE_LEN 8

#define READINGERROR "ERROR1"
#define READINGERROR_LEN 6

#define INVALIDCOMMAND "ERROR2"
#define INVALIDCOMMAND_LEN 6

#define ALREADYINSESSION "ERROR3" // cannot create account while in session
#define ALREADYINSESSION_LEN 6

#define ACCOUNTEXISTS "ERROR4"
#define ACCOUNTEXISTS_LEN 6

#define ACCOUNTDOESNTEXIST "ERROR5"
#define ACCOUNTDOESNTEXIST_LEN 6

#define ACCOUNTINUSE "ERROR6"
#define ACCOUNTINUSE_LEN 6

#define NOACTIVESESSION "ERROR7"
#define NOACTIVESESSION_LEN 6

#define NEGATIVEDEPOSIT "ERROR8"
#define NEGATIVEDEPOSIT_LEN 6

#define OVERDRAW "ERROR9"
#define OVERDRAW_LEN 6


//// Success
#define ACCOUNTSUCCESS "Account created successfully"
#define ACCOUNTSUCCESS_LEN 28

#define RETRIEVESUCCESS "Account retrieved successfully"
#define RETRIEVESUCCESS_LEN 30

#define SESSIONENDED "Session ended successfully"
#define SESSIONENDED_LEN 26


void error(char* msg) {
    perror(msg);
    exit(1);
}

struct l_l;

typedef struct tnd {
    struct tnd* prev;
    struct tnd* next;
    pthread_t t_id;
    int newsocket_fd;
    struct l_l *parent_list;
    bool die;
    pthread_mutex_t die_lock;
} thread_node;

typedef struct l_l {
    thread_node* head;
    pthread_mutex_t mutex;
    int size;
} linked_list;

thread_node* create_thread_node(linked_list* list) {
    thread_node* node = (thread_node*) malloc(sizeof(thread_node));
    pthread_mutex_lock(&(list->mutex));
    if(list->head != NULL)
        list->head->prev = node;
    node->prev = NULL;
    node->next = list->head;
    list->head = node;
    node->parent_list = list;
    node->die = false;
    list->size += 1;
    pthread_mutex_unlock(&(list->mutex));
    return node;
}


void delete_node(thread_node* node) {
    if(node == NULL) {
        error("Can't delete a null node.");
    }
    linked_list* temp = node->parent_list;
    pthread_mutex_lock(&(temp->mutex));
    if(node->prev != NULL)
        node->prev->next = node->next;
    if(node->next != NULL)
        node->next->prev = node->prev;
    if(node->parent_list->head == node) {
        node->parent_list->head = node->next;
    }
    node->parent_list->size -= 1;
    free(node);
    pthread_mutex_unlock(&(temp->mutex));
}

void strip(char* inop) {
    int l = strlen(inop) - 1;
    while(l >= 0 && (inop[l] == '\n' || inop[l] == '\r')) l--;
    inop[l + 1] = '\0';
}

#endif