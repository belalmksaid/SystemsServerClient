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


#define SHUTDOWNMESSAGE "SHUTDOWN\n"
#define SHUTDOWNMESSAGE_LEN 9


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
        list->head->next = node;
    node->prev = list->head;
    list->head = node;
    node->parent_list = list;
    node->next = NULL;
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
        node->parent_list->head = NULL;
    }
    node->parent_list->size -= 1;
    free(node);
    pthread_mutex_unlock(&(temp->mutex));
}

#endif