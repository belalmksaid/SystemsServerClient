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

#endif