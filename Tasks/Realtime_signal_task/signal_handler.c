// Include standard header files
#include <string.h>   // String operations (not used in this code but included)
#include <signal.h>   // For signal handling and signal-related functions
#include <stdio.h>    // Standard I/O functions (e.g., printf, perror)
#include <unistd.h>   // For system calls like getpid() and pause()
#include <stdlib.h>   // General utilities (e.g., exit())
#include <sys/types.h> // For data types like pid_t (used in getpid)

// Static handler function to terminate the process with a specific real-time signal (SIGRTMIN)
static void handler1 (int sig) // Signal handler for SIGRTMIN
{
    printf("\nYou send Real time signal 0 process terminate \n"); // Notify signal received and process is terminating.

    // Send the SIGINT signal (Interrupt signal) to the current process (self-termination)
    if (kill(getpid(), SIGINT) == -1) // kill() sends a signal to the process ID specified by getpid()
    {
        perror("kill: \n"); // Print an error message if kill() fails
    }
}

// Static handler function to terminate with a core dump file
static void handler2 (int sig) // Signal handler for SIGRTMIN+1
{
    // Print instructions on how to generate a core dump if not generated automatically
    printf("if the core dump is not generated do this \n");
    printf("ulimit -c unlimited \nsudo service apport stop ( 3shan ytl3lk f nfs mkan al file) "); 
    printf("\nYou send Real time signal 1 process terminate with core dump file \n");

    // Send the SIGABRT signal (Abort signal) to the current process to force termination and create a core dump
    if (kill(getpid(), SIGABRT) == -1) // kill() sends SIGABRT to self
    {
        perror("kill: \n"); // Print an error message if kill() fails
    }
}

// Main function
int main() {

    // Register signal handlers for real-time signals
    signal(SIGRTMIN, handler1);       // SIGRTMIN (signal 34) is handled by handler1
    signal(SIGRTMIN + 1, handler2);   // SIGRTMIN+1 (signal 35) is handled by handler2

    // Print instructions for the user
    printf("You can send from another terminal using kill command \n");
    printf("kill -34 %d  to terminate \n", getpid()); // Print the process ID and how to send SIGRTMIN (signal 34)
    printf("kill -35 %d  to terminate and generate core dump \n", getpid()); // Print process ID for SIGRTMIN+1 (signal 35)

    // Keep the process running indefinitely to wait for signals
    while (1); // Infinite loop; the process waits for signals here
}

