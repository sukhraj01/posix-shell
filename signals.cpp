#include "header.h"

void handleSigInt(int /*sig*/) {
    if (fgPid != -1) {
        kill(fgPid, SIGINT);   // send SIGINT to foreground job
        write(STDOUT_FILENO, "\n", 1);
    }
    // if fgPid == -1, do nothing (shell stays alive)
}

void handleSigTstp(int /*sig*/) {
    if (fgPid != -1) {
        kill(fgPid, SIGTSTP);  // send SIGTSTP to foreground job
        write(STDOUT_FILENO, "\n", 1);
    }
    // if fgPid == -1, do nothing
}

// Install handlers for SIGINT and SIGTSTP
void signalSetup() {
    signal(SIGINT, handleSigInt);
    signal(SIGTSTP, handleSigTstp);
}