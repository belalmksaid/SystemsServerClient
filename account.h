#ifndef ACCOUNT_H
#define ACCOUNT_H

#define session_flag char
#define IN_SESSION 'i'
#define NOT_IN_SESSION 'o'

#define BANKSIZE 1000

#include "commons.h"

typedef struct {
    char* name;
    double balance;
    session_flag session;
    pthread_mutex_t lock;

} account;

typedef struct h_b {
    struct h_b *next;
    int value;
    char* key;
} hash_bit;

typedef struct {
    account* accounts;
    hash_bit* account_hash;
    int size;
    int max_size;
    pthread_mutex_t bank_lock;
} bank;

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

account* get_account(bank* bk, char* name) {
    uint map = hash(name) % BANKSIZE;
    hash_bit* trav = &(bk->account_hash[map]);
    if(trav->key == NULL) return NULL;
    while(trav != NULL && strcmp(trav->key, name) != 0)
        trav = trav->next;
    if(trav == NULL) return NULL;
    return &(bk->accounts[trav->value]);
}

uint add_to_map(bank* bk, char* name, int index) {
    uint map = hash(name) % BANKSIZE;
    if(bk->account_hash[map].key == NULL) {
        bk->account_hash[map].key = name;
        bk->account_hash[map].value = index;
    }
    else {
        hash_bit* trav = &(bk->account_hash[map]);
        while(trav->next != NULL)
            trav = trav->next;
        trav->next = (hash_bit*)malloc(sizeof(hash_bit));
        trav->next->key = name;
        trav->next->value = index;
    }
    return map;
}


int add_account(bank* bk, char* name, double balance, session_flag flag) {
    if(get_account(bk, name) != NULL) {
        return -1; // account already exists
    }
    pthread_mutex_lock(&(bk->bank_lock));
    if(bk->size == bk->max_size) {
        bk->max_size *= 2;
        bk->accounts = (account*)realloc(bk->accounts, bk->max_size * sizeof(account));
    }
    bk->accounts[bk->size].name = name;
    bk->accounts[bk->size].balance = balance;
    bk->accounts[bk->size].session = flag;
    add_to_map(bk, name, bk->size);
    bk->size += 1;
    pthread_mutex_unlock(&(bk->bank_lock));
    return 1;
}

bank create_bank() {
    bank bk;
    bk.size = 0;
    bk.max_size = BANKSIZE;
    bk.accounts = (account*) malloc(sizeof(account) * bk.max_size);
    bk.account_hash = (hash_bit*) calloc(bk.max_size, sizeof(hash_bit));
    return bk;
} 

#endif