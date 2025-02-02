#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/* 
   Signal handler for SIGCHLD to prevent zombie processes.
   This function is triggered whenever a child process terminates.
*/
void sigchld_handler(int sig) {
    (void)sig;  /* Unused parameter to avoid compiler warnings */
    int status;
    pid_t pid;

    /* 
       Reap all terminated child processes using waitpid.
       WNOHANG ensures that the function does not block if no child has exited.
    */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            /* 
               If the child terminated normally, print its PID and exit status.
            */
            printf("Child process %d exited normally with status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            /* 
               If the child was terminated by a signal, print its PID and termination signal.
            */
            printf("Child process %d terminated by signal %d\n", pid, WTERMSIG(status));
        }
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;  /* Assigning the signal handler function */
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;  /* Restart interrupted syscalls, ignore stopped children */
    sigemptyset(&sa.sa_mask);  /* Initialize signal mask to block no signals */
    sigaction(SIGCHLD, &sa, NULL);  /* Register SIGCHLD handler */

    /* Print the parent process ID */
    printf("Parent PID: %d\n", getpid());

    /* Fork multiple child processes */
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            /* Child process */
            printf("Child PID: %d started\n", getpid());
            sleep(30);  /* Simulate child process execution time */
            exit(100 + i);  /* Exit with a unique status */
        }
    }

    /* Parent process loop to keep it running and handling SIGCHLD */
    while (1) {
        sleep(1);  /* Keep the parent process alive indefinitely */
    }

    return 0;
}


