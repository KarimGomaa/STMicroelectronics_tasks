#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/* Function to display usage instructions */
void print_usage(const char *progname) {
    printf("Usage: %s <PID> <level>\n", progname);
    printf("Levels: disable, error, warning, info, debug\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID.\n");
        return 1;
    }

    int signal_to_send = 0;

    /* Determine which signal to send */
    if (strcmp(argv[2], "disable") == 0) signal_to_send = SIGRTMIN;
    else if (strcmp(argv[2], "error") == 0) signal_to_send = SIGRTMIN+1;
    else if (strcmp(argv[2], "warning") == 0) signal_to_send = SIGRTMIN+2;
    else if (strcmp(argv[2], "info") == 0) signal_to_send = SIGRTMIN+3;
    else if (strcmp(argv[2], "debug") == 0) signal_to_send = SIGRTMIN+4;
    else {
        fprintf(stderr, "Invalid log level.\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Send the signal */
    if (kill(pid, signal_to_send) == -1) {
        perror("Failed to send signal");
        return 1;
    }

    printf("Log level change signal sent to PID %d.\n", pid);
    return 0;
}

