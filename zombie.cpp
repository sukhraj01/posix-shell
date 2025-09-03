#include "header.h"

// Signal handler to reap zombie processes
void reap_zombies(int sig) {
    (void)sig;
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0) { // WNOHANG returns 0 if no child left
        // Child process was reaped successfully.
        // You could add a message here for debugging if needed.
    }
    errno = saved_errno;
}

// Function to set up the signal handler
void initializeZombieCleaner() {
    struct sigaction sa;
    sa.sa_handler = reap_zombies;
    sigemptyset(&sa.sa_mask); // dont block any other signal while reap_zombie is working
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("sigaction failed");
    }
}