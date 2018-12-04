
#include "commons.h"
#include "account.h"
#include <signal.h>

volatile bool INTERRUPTED = false;

void SIGINT_HANDLER(int d) {
    INTERRUPTED = true;
}

int main(int argc, char** argv) {
    signal(SIGINT, SIGINT_HANDLER);
    while(!INTERRUPTED) {

    }
    return 0;
}