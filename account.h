#ifndef ACCOUNT_H
#define ACCOUNT_H

#define session_flag char
#define IN_SESSION 'i'
#define NOT_IN_SESSION 'o'

#include "commons.h"

typedef struct  
{
    char* name;
    float balance;
    session_flag session;
    pthread_mutex_t lock;

} account;


#endif