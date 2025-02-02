#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>

/* Define log levels */
typedef enum {
    LOG_DISABLED = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
} LogLevel;

LogLevel current_log_level = LOG_INFO; /* Default log level */

/* Log level names */
const char *log_level_names[] = {"DISABLED", "ERROR", "WARNING", "INFO", "DEBUG"};

/* Function to handle signals and change log level */
void signal_handler(int signo) {
    if (signo == SIGRTMIN) {
        current_log_level = LOG_DISABLED;
    } else if (signo == SIGRTMIN+1) {
        current_log_level = LOG_ERROR;
    } else if (signo == SIGRTMIN+2) {
        current_log_level = LOG_WARNING;
    } else if (signo == SIGRTMIN+3) {
        current_log_level = LOG_INFO;
    } else if (signo == SIGRTMIN+4) {
        current_log_level = LOG_DEBUG;
    }
    printf("Log level changed to: %s\n", log_level_names[current_log_level]);
}

/* Logging function */
void log_message(LogLevel level, const char *format, ...) {
    if (level > current_log_level) return; /* Ignore logs above current level */

    va_list args;
    va_start(args, format);
    printf("[%s] ", log_level_names[level]);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/* Logging macros */
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)

int main() {
    /* Register signal handlers */
    signal(SIGRTMIN, signal_handler);
    signal(SIGRTMIN+1, signal_handler);
    signal(SIGRTMIN+2, signal_handler);
    signal(SIGRTMIN+3, signal_handler);
    signal(SIGRTMIN+4, signal_handler);


    printf("Logger started. PID: %d\n", getpid());
    printf("- Use SIGRTMIN to Disable logger.\n- Use SIGRTMIN+1 to set looger to error.\n");
    printf("- Use SIGRTMIN+2 set looger to warning.\n- Use SIGRTMIN+3 to set looger to info.\n- Use SIGRTMIN+4 to set looger to debug.\n");
    printf("---------------------------------------------------- \n");

    /* Simulated logging loop */
    while (1) {
        LOG_ERROR("This is an ERROR message.");
        LOG_WARNING("This is a WARNING message.");
        LOG_INFO("This is an INFO message.");
        LOG_DEBUG("This is a DEBUG message.");
	printf("---------------------------------------------------- \n");
	sleep(3);
    }

    return 0;
}

