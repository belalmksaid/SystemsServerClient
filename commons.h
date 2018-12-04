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

void error(char* msg) {
    perror(msg);
    exit(1);
}

struct l_l;

typedef struct tnd {
    struct tnd* prev;
    struct tnd* next;
    int t_id;
    int newsocket_fd;
    struct l_l *parent_list;
} thread_node;

typedef struct l_l {
    thread_node* head;
    pthread_mutex_t mutex;
} linked_list;

thread_node* create_thread_node(linked_list* list) {
    thread_node* node = (thread_node*) malloc(sizeof(thread_node));
    pthread_mutex_lock(&(list->mutex));
    list->head->next = node;
    node->prev = list->head;
    list->head = node;
    node->parent_list = list;
    node->next = NULL;
    pthread_mutex_unlock(&(list->mutex));
    return node;
}

void delete_node(thread_node* node) {
    pthread_mutex_lock(&(node->parent_list->mutex));
    if(node->prev != NULL)
        node->prev->next = node->next;
    if(node->next != NULL)
        node->next->prev = node->prev;
    free(node);
    pthread_mutex_unlock(&(node->parent_list->mutex));
}

#endif