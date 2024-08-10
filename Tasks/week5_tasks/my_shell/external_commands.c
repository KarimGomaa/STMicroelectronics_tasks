/******************************************************************************
 *
 * File Name: external_commands.c
 *
 * Description: Source file for the external commands and contain one internal command which is PHIST
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "external_commands.h"
#include "my_shell.h"  /* To get input_string which is used in fun_EXTERNAl_COMMAND */

static ProcessInfo process_history[MAX_HISTORY]; /* Global array to store process history */
static int process_count = 0; /* Counter for the number of processes in the history */
static char redirection_error_occur = 0; /* Flag to indicate if a redirection error occurred */
static char pipeline_exit_now = 0; /* Flag to indicate if a pipeline exit is needed */
static int num_of_commands; /* Number of commands */

/*
 * Description :
 * 1. Function to add a process in the array process_history that we created.
 */
static void add_process(int pid, int exit_status, char *process_name)
{
    if (process_count < MAX_HISTORY) {
        process_history[process_count].pid = pid; /* Store the process ID */
        process_history[process_count].exit_status = exit_status; /* Store the exit status */
        strncpy(process_history[process_count].process_name, process_name, MAX_PROCESS_NAME - 1); /* Store the process name */
        process_history[process_count].process_name[MAX_PROCESS_NAME - 1] = '\0'; /* Ensure null-termination */
        process_count++; /* Increment the process count */
    } else {
        /* Shift history to make room for new entry */
        memmove(process_history, process_history + 1, (MAX_HISTORY - 1) * sizeof(ProcessInfo));
        process_history[MAX_HISTORY - 1].pid = pid; /* Store the new process ID at the end */
        process_history[MAX_HISTORY - 1].exit_status = exit_status; /* Store the exit status */
        strncpy(process_history[MAX_HISTORY - 1].process_name, process_name, MAX_PROCESS_NAME - 1); /* Store the process name */
        process_history[MAX_HISTORY - 1].process_name[MAX_PROCESS_NAME - 1] = '\0'; /* Ensure null-termination */
    }
}

/*
 * Description :
 * 1. Function to be called to display the last 10 processes with their PID, exit status, and name.
 * 2. It is an internal command and is implemented here to display external commands processes.
 */
void fun_PHIST() 
{
    printf("Last %d processes:\n", process_count); /* Display the count of recent processes */
    printf("0 means: success, Other numbers mean: error \n");
    printf("----------------------------------------------\n");
    for (int i = 0; i < process_count; i++) {
        printf("PID: %d,     Exit Status: %d,      Process Name: %s\n", process_history[i].pid, process_history[i].exit_status, process_history[i].process_name);
    }
}

/*
 * Description :
 * 1. Function to check if a command is external or not.
 */
int is_external_command(const char *command) 
{
    char *path = getenv("PATH"); /* Get the PATH environment variable */
    char *token;
    char full_path[512]; /* Buffer to store the full path of the command */

    /* Check if the user sent a full path */
    if (command[0] == '/') {
        if (access(command, X_OK) == 0) {
            return 1; /* Command is executable */
        }
    }

    if (path == NULL) {
        return 0; /* No PATH environment variable found */
    }

    /* Duplicate the PATH string so we can use strtok without affecting the original */
    char *path_dup = strdup(path);
    token = strtok(path_dup, ":");

    while (token != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command); /* Construct the full path */
        if (access(full_path, X_OK) == 0) {
            free(path_dup);
            return 1; /* Command is executable */
        }
        token = strtok(NULL, ":");
    }

    free(path_dup);
    return 0; /* Command not found in any PATH directory */
}

/*
 * Description :
 * 1. Function to be called if you want to make input/output or error redirection.
 */
static void parse_redirection(char *str, char *redirect_pos, char **redirected_file, char is_error) {

    /* Skip the '>' character and any spaces following it */
    char *file_start;
    if (is_error) {
        file_start = redirect_pos + 2; /* Skip '2' characters if it's an error redirection '2>' */
    } else {
        file_start = redirect_pos + 1; /* Skip '1' character if it's a regular redirection '>' */
    }

    while (*file_start == ' ') {
        file_start++; /* Skip spaces */
    }

    /* Check if there's nothing after '>' */
    if (*file_start == '\0' || *file_start == '>' || *file_start == '<') {
        *redirected_file = NULL; /* Set redirected_file to NULL if the filename is invalid */
        redirection_error_occur = 1; /* Set error flag */
        return;
    } 

    /* Extract the filename until the next space or end of string */
    char *file_end = file_start;
    while (*file_end != ' ' && *file_end != '\0') {
        file_end++;
    }

    /* Duplicate the filename and store it in redirected_file */
    *redirected_file = strndup(file_start, file_end - file_start);

    /* Check if the filename contains '>' */
    char *check_redirected_file = strstr(*redirected_file, ">");
    if (check_redirected_file != NULL) {
        *redirected_file = NULL; /* Set redirected_file to NULL if '>' is found */
        redirection_error_occur = 1; /* Set error flag */
        return;
    }

    /* Check if the filename contains '<' */
    check_redirected_file = strstr(*redirected_file, "<");
    if (check_redirected_file != NULL) {
        *redirected_file = NULL; /* Set redirected_file to NULL if '<' is found */
        redirection_error_occur = 1; /* Set error flag */
        return;
    }

    /* Shift the remaining part of the string to remove "> filename" */
    memmove(redirect_pos, file_end, strlen(file_end) + 1);

    /* Trim any trailing spaces from the modified command */
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') {
        str[len - 1] = '\0'; /* Remove trailing spaces */
        len--;
    }
}

/*
 * Description :
 * 1. Frees the memory allocated for the argv array.
 */
void free_argv(char ***argv) {
    if (argv != NULL)
    {
        /* Loop through each command in argv */
        for (int k = 1; k < num_of_commands; k++) {
            /* Check if the argv[k] pointer is valid */
            if (argv[k] != NULL) {
                /* Loop through each argument in the command and free it */
                int i = 0;
                while (argv[k][i] != NULL)
                {
                    free(argv[k][i]); /* Free each argument */
                    i++;
                }
                /* Free the array of arguments itself */
                free(argv[k]);
            }
        }
        /* Finally, free the array of command pointers */
        free(argv);
    }
}

/*
 * Description :
 * 1. Parses the redirection part of the command for pipelining.
 * 2. Extracts the filename and updates the input string.
 */
static void parse_pipeling_first_step(char *str, char *redirect_pos, char **redirected_file) {

    /* Skip the '>' character and any spaces following it */
    char *file_start = redirect_pos + 1;
    while (*file_start == ' ') {
        file_start++;
    }

    /* Check if there's nothing after '>' */
    if (*file_start == '\0' || *file_start == '>' || *file_start == '<') {
        *redirected_file = NULL;
        pipeline_exit_now = 1; /* Set error flag */
        return;
    } 

    /* Extract the filename until the next | or end of string */
    char *file_end = file_start;
    while (*file_end != '|' && *file_end != '\0') {
        file_end++;
    }

    /* Duplicate the filename and store it in redirected_file */
    *redirected_file = strndup(file_start, file_end - file_start);

    /* Check if the filename contains '>' or '<' */
    char *check_redirected_file = strstr(*redirected_file, ">");
    if (check_redirected_file != NULL)
    {
        *redirected_file = NULL;
        pipeline_exit_now = 1; /* Set error flag */
        return;
    }

    check_redirected_file = strstr(*redirected_file, "<");
    if (check_redirected_file != NULL)
    {
        *redirected_file = NULL;
        pipeline_exit_now = 1; /* Set error flag */
        return;
    }

    /* Shift the remaining part of the string to remove "> filename" */
    memmove(redirect_pos, file_end, strlen(file_end) + 1);

    /* Trim any trailing spaces from the modified command */
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') {
        str[len - 1] = '\0';
        len--;
    }
}

/*
 * Description :
 * 1. Parses and processes each command in the pipeline.
 * 2. Counts tokens and copies them into the argv array.
 */
static void parse_pipeling_second_step(char ***argv, int *argc, int k, char *command_buff)
{
    char command_buffer_counting_argv[100];
    char command_buffer_copy_to_argv[100];
    strcpy(command_buffer_counting_argv, command_buff); /* Copy command buffer for counting arguments */
    strcpy(command_buffer_copy_to_argv, command_buff); /* Copy command buffer for tokenization */
    argc[k] = 0;
    char *token;

    /* Count the number of tokens considering backslash-escaped spaces */
    token = strtok(command_buffer_counting_argv, " ");
    while (token != NULL) {
        while (token[strlen(token) - 1] == '\\') {
            token[strlen(token) - 1] = '\0'; /* Remove the trailing backslash */
            strcat(token, " ");
            char *next_part = strtok(NULL, " ");
            if (next_part != NULL) {
                strcat(token, next_part);
            } else {
                break;
            }
        }
        argc[k]++; /* Increment argument count */
        token = strtok(NULL, " ");
    }

    /* Allocate memory for argv array (+1 for NULL terminator) */
    argv[k] = malloc((argc[k] + 1) * sizeof(char *));
    if (argv[k] == NULL) {
        perror("malloc"); /* Print error if memory allocation fails */
        return;
    }

    /* Copy tokens into argv */
    token = strtok(command_buffer_copy_to_argv, " ");
    for (int i = 0; i < argc[k]; i++) {
        /* Handle escaped spaces during copying */
        while (token[strlen(token) - 1] == '\\') {
            token[strlen(token) - 1] = '\0'; /* Remove the trailing backslash */
            strcat(token, " ");
            char *next_part = strtok(NULL, " ");
            if (next_part != NULL) {
                strcat(token, next_part);
            } else {
                break;
            }
        }

        argv[k][i] = malloc(strlen(token) + 1);
        if (argv[k][i] == NULL) {
            perror("malloc"); /* Print error if memory allocation fails */
            /* Free previously allocated memory before exiting */
            for (int j = 0; j < i; j++) {
                free(argv[k][j]);
            }
            free(argv[k]);
            return;
        }
        strcpy(argv[k][i], token); /* Copy token to argv */
        token = strtok(NULL, " ");
    }
    argv[k][argc[k]] = NULL; /* Null-terminate the argv array */
}


/*
 * Description :
 * 1. Function to handle pipelining in the command input.
 * 2. This function executes multiple commands connected by pipes.
 */
void fun_PIPELINING(char *pipelining_delimeter)
{
    char input_string_copy[INPUT_BUFFER];
    strcpy(input_string_copy, input_string); /* Copy the input string for processing */

    num_of_commands = 2; /* Initialize number of commands for pipelining */
    char *count = pipelining_delimeter + 1;
    count = strstr(count, "|"); /* Find the next '|' character in the command */
    while (count != NULL) {
        num_of_commands++; /* Increment command count for each '|' found */
        count = strstr(count + 1, "|"); /* Find subsequent '|' characters */
    }

    char ***argv; /* Array of char** pointers for commands */
    /* Allocate memory for argv */
    argv = malloc(num_of_commands * sizeof(char **));
    if (argv == NULL) {
        perror("malloc"); /* Print error if memory allocation fails */
        exit(EXIT_FAILURE); /* Exit the program with failure status */
    }

    int argc[num_of_commands]; /* Array to hold argument counts for each command */
    int k;
    char *command_buff;

    for (k = 1; k < num_of_commands; k = k + 1)
    {
        parse_pipeling_first_step(input_string, pipelining_delimeter, &command_buff); /* Parse the first part of the pipelined command */
        if (pipeline_exit_now)
        {
            pipeline_exit_now = 0;
            printf("Error in the command given after the | , Please make sure to write it correctly\n"); /* Print error if parsing fails */
            if (k > 1) {
                free_argv(argv); /* Free allocated memory if error occurs */
            }
            return; /* Exit the function */
        }
        parse_pipeling_second_step(argv, argc, k, command_buff); /* Parse the second part and store command in argv */
        pipelining_delimeter = strstr(input_string, "|"); /* Update the delimiter to find the next command */
    }

    char *first_part_of_input_string = strtok(input_string_copy, "|"); /* Get the first command in the pipeline */
    parse_pipeling_second_step(argv, argc, 0, first_part_of_input_string); /* Parse and store the first command */

    for (int j = 1; j < num_of_commands; j++)
    {
        if (is_external_command(argv[j][0]) == 0)
        {
            printf("Error in the command given after the | , No external command found with this name\n"); /* Print error if command is not found */
            free_argv(argv); /* Free allocated memory */
            return; /* Exit the function */
        }
    }

    int pipe_fd[2];  /* File descriptors for the pipe */
    int prev_fd = 0; /* File descriptor for the previous command's output */
    int pids[num_of_commands];  /* Array to store PIDs of child processes */

    for (int i = 0; i < num_of_commands; i++) {
        /* Create a pipe for the current command */
        if (i < num_of_commands - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe failed"); /* Print error if pipe creation fails */
                exit(1); /* Exit the program with failure status */
            }
        }

        /* Fork a process for the current command */
        int pid = fork();
        if (pid == -1) {
            perror("fork failed"); /* Print error if fork fails */
            exit(1); /* Exit the program with failure status */
        } else if (pid == 0) {
            /* In the child process */

            /* Redirect input from the previous command */
            if (prev_fd != 0) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd); /* Close the previous file descriptor */
            }

            /* Redirect output to the next command */
            if (i < num_of_commands - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]); /* Close the write end of the pipe */
            }

            /* Close unused pipe read end */
            if (i < num_of_commands - 1) {
                close(pipe_fd[0]); /* Close the read end of the pipe */
            }

            /* Execute the command */
            if (argv[i][0][0] != '/')
            {
                execvp(argv[i][0], argv[i]); /* Execute command using execvp */
                perror("execvp failed"); /* Print error if execvp fails */
                exit(1); /* Exit the program with failure status */
            }
            else
            {
                execv(argv[i][0], argv[i]); /* Execute command using execv */
                perror("execv failed"); /* Print error if execv fails */
                exit(1); /* Exit the program with failure status */
            }
        } else {
            /* In the parent process */

            /* Store the PID of the child process */
            pids[i] = pid;

            /* Close the write end of the current pipe */
            if (i < num_of_commands - 1) {
                close(pipe_fd[1]);
            }

            /* Close the read end of the previous pipe (if any) */
            if (prev_fd != 0) {
                close(prev_fd);
            }

            /* Save the read end of the pipe for the next command */
            if (i < num_of_commands - 1) {
                prev_fd = pipe_fd[0];
            }
        }
    }

    /* Wait for all child processes to finish and get their statuses */
    int status;
    for (int i = 0; i < num_of_commands; i++) {
        waitpid(pids[i], &status, 0); /* Wait for the child process to complete */
        if (WIFEXITED(status)) {

            char *process_name = argv[i][0];
            if (process_name[0] == '/') {
                /* Find the last occurrence of '/' */
                char *last_slash = strrchr(process_name, '/');
                if (last_slash != NULL) {
                    /* Move the pointer to the next character after the last '/' */
                    process_name = last_slash + 1;
                }
            }
            add_process(pids[i], WEXITSTATUS(status), process_name); /* Add the process to the process list */
        }
    }
    free_argv(argv); /* Free allocated memory for argv */
}


/*
 * Description :
 * 1. Function to be called if `is_external_command` is true.
 * 2. You can call external commands by their name or their paths.
 */
void fun_EXTERNAL_COMMAND()
{

    /* Buffers for processing the input string without modifying the original */
    char input_string_func_for_counting_tokens[INPUT_BUFFER];
    char input_string_func_copy_tokens_to_argv[INPUT_BUFFER];

    char *output_file = NULL; /* Pointer for output redirection file */
    char *input_file = NULL; /* Pointer for input redirection file */
    char *error_file = NULL; /* Pointer for error redirection file */

    /* Check for pipelining in the input string */
    char *pipelining = strstr(input_string, "|");  /* Find the '|' character */
    if(pipelining != NULL)
    {
        fun_PIPELINING(pipelining); /* Handle pipelining if present */
        return;
    }

    /* Check for error redirection in the input string */
    char *redirect_pos_error = strstr(input_string, "2>");  /* Find the '2>' character */
    if(redirect_pos_error != NULL)
    {
        parse_redirection(input_string, redirect_pos_error, &error_file, ERROR_REDIRECTION); /* Parse error redirection */
        if(redirection_error_occur)
        {
            redirection_error_occur = 0;
            printf("Error Redirection Failed please enter a valid File Name or File Path \n"); /* Print error message */
            return;
        }
    }

    /* Check for output redirection in the input string */
    char *redirect_pos_output = strstr(input_string, ">");  /* Find the '>' character */
    if(redirect_pos_output != NULL)
    {
        parse_redirection(input_string, redirect_pos_output, &output_file, OUTPUT_REDIRECTION); /* Parse output redirection */
        if(redirection_error_occur)
        {
            redirection_error_occur = 0;
            printf("Output Redirection Failed please enter a valid File Name or File Path \n"); /* Print error message */
            return;
        }
    }

    /* Check for input redirection in the input string */
    char *redirect_pos_input = strstr(input_string, "<");  /* Find the '<' character */
    if(redirect_pos_input != NULL)
    {    
        parse_redirection(input_string, redirect_pos_input, &input_file, INPUT_REDIRECTION); /* Parse input redirection */
        if(redirection_error_occur)
        {
            redirection_error_occur = 0;
            printf("Input Redirection Failed please enter a valid File Name or File Path \n"); /* Print error message */
            return;
        }
    }

    /* Copy input string for processing */
    strcpy(input_string_func_for_counting_tokens, input_string);
    strcpy(input_string_func_copy_tokens_to_argv, input_string);

    /* Extract the process name from the input string */
    char *process_name = strtok(input_string, " ");
    /* Check if the process_name starts with '/' */
    if (process_name[0] == '/') {
        /* Find the last occurrence of '/' */
        char *last_slash = strrchr(process_name, '/');
        if (last_slash != NULL) {
            /* Move the pointer to the next character after the last '/' */
            process_name = last_slash + 1;
        }
    }

    char *token;
    int argc = 0; /* Argument count */
    char **argv; /* Argument vector */
    int pid; /* Process ID */
    int status; /* Process status */

    /* Count the number of tokens considering backslash-escaped spaces */
    token = strtok(input_string_func_for_counting_tokens, " ");
    while (token != NULL) {
        while (token[strlen(token) - 1] == '\\') {
            token[strlen(token) - 1] = '\0'; /* Remove the trailing backslash */
            strcat(token, " ");
            char *next_part = strtok(NULL, " ");
            if (next_part != NULL) {
                strcat(token, next_part);
            } else {
                break;
            }
        }
        argc++;
        token = strtok(NULL, " ");
    }

    /* Allocate memory for argv array (+1 for NULL terminator) */
    argv = malloc((argc + 1) * sizeof(char *));
    if (argv == NULL) {
        perror("malloc"); /* Print error if memory allocation fails */
        return;
    }

    /* Copy tokens into argv */
    token = strtok(input_string_func_copy_tokens_to_argv, " ");
    for (int i = 0; i < argc; i++) {
        /* Handle escaped spaces during copying */
        while (token[strlen(token) - 1] == '\\') {
            token[strlen(token) - 1] = '\0'; /* Remove the trailing backslash */
            strcat(token, " ");
            char *next_part = strtok(NULL, " ");
            if (next_part != NULL) {
                strcat(token, next_part);
            } else {
                break;
            }
        }

        argv[i] = malloc(strlen(token) + 1); /* Allocate memory for each argument */
        if (argv[i] == NULL) {
            perror("malloc"); /* Print error if memory allocation fails */
            /* Free previously allocated memory before exiting */
            for (int j = 0; j < i; j++) {
                free(argv[j]);
            }
            free(argv);
            return;
        }
        strcpy(argv[i], token); /* Copy the token to argv */
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL; /* Null-terminate the argv array */

    char is_execvp; /* Flag to determine which exec function to use */
    if(argv[0][0] != '/')
    {
        is_execvp = 1; /* Use execvp if the command is not an absolute path */
    } else {
        is_execvp = 0; /* Use execv if the command is an absolute path */
    }
    /* Fork a new process */
    pid = fork();
    if (pid == -1) {
        perror("fork"); /* Print error if fork fails */
        /* Free memory before exiting */
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);
        return;
    } 
    else if (pid == 0) 
    {
        /* Child process */

        /* Handle input redirection */
        if (input_file != NULL) {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0) {
                perror("Failed to open input file"); /* Print error if file cannot be opened */
                exit(1);
            }
            dup2(input_fd, STD_IN); /* Redirect standard input */
            close(input_fd); /* Close the file descriptor */
        }

        /* Handle output redirection */
        if (output_file != NULL) {
            int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0) {
                perror("Failed to open output file"); /* Print error if file cannot be opened */
                exit(1);
            }
            dup2(output_fd, STD_OUT); /* Redirect standard output */
            close(output_fd); /* Close the file descriptor */
        }

        /* Handle error redirection */
        if (error_file != NULL) {
            int error_fd = open(error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (error_fd < 0) {
                perror("Failed to open error file"); /* Print error if file cannot be opened */
                exit(1);
            }
            dup2(error_fd, STD_ERROR); /* Redirect standard error */
            close(error_fd); /* Close the file descriptor */
        }

        if(is_execvp){
            /* Execute the command with execvp */
            if (execvp(argv[0], argv) == -1) {
                perror("execvp"); /* Print error if execvp fails */
                /* Free memory if exec fails */
                for (int i = 0; i < argc; i++) {
                    free(argv[i]);
                }
                free(argv);
                exit(1);
            }
        } else {
            /* Execute the command with execv */
            if (execv(argv[0], argv) == -1) {
                perror("execv"); /* Print error if execv fails */
                /* Free memory if exec fails */
                for (int i = 0; i < argc; i++) {
                    free(argv[i]);
                }
                free(argv);
                exit(1);
            }
        }
    } 
    else 
    {
        /* Parent process */
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid"); /* Print error if waitpid fails */
            return;
        }

        /* Process exit status */
        if (WIFEXITED(status)) {
            add_process(pid, WEXITSTATUS(status), process_name); /* Record the process status */
        }

        /* Free memory */
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);
    }
}
