/*******************************************************************************************************
 *
 * File Name: my_shell.c
 *
 * Description: Shell Program used to do the Following commands (mypwd,myecho,mycp,mymv,help,type,envir,cd,exit and all external commands)
 *
 * Author: Karim Gomaa
 *
 ********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "my_shell.h"
#include "internal_commands.h"
#include "external_commands.h"

char input_string[INPUT_BUFFER]; /* Global buffer to hold the command input and used across all functions */

/*
 * Description :
 * 1. Function to print an error message for the user.
 */
void ERROR_MESSAGE()
{
    fflush(stdout); /* Flush the output buffer to ensure the message is printed */
    printf("Please Enter a valid command -_- use help to know all commands \n"); /* Print error message */
}

/*
 * Description :
 * 1. Function used to remove the "set" prefix when trying to insert an environmental variable.
 */
void remove_set_prefix(char *str) {
    const char *prefix = "set "; /* Prefix to remove from the string */
    char *pos = strstr(str, prefix); /* Find the position of the prefix in the string */
    
    if (pos != NULL) { /* If the prefix is found */
        // Move the pointer past the "set " prefix
        char *new_start = pos + strlen(prefix); /* Calculate new start position */
        memmove(str, new_start, strlen(new_start) + 1); /* Move the remaining string to the start */
    }
}

/*
 * Description :
 * 1. Function to remove leading spaces from the input string.
 */
void REMOVE_LeadingSpaces(char *str)
{
    int i = 0; /* Index for iterating through the string */
    int j = 0; /* Index for writing characters to the string */

    while (str[i] == ' ') {
        i++; /* Increment index until a non-space character is found */
    }

    while (str[i]) {
        str[j++] = str[i++]; /* Copy characters to the beginning of the string */
    }

    str[j] = '\0'; /* End the string with a null terminator */
}

/*
 * Description :
 * 1. Function to remove the newline character from the input string.
 */
void remove_newline(char *str)
{
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) { /* Find the newline character in the string */
        *pos = '\0'; /* Replace newline with null terminator */
    }
}

/*
 * Description :
 * 1. Function to write the shell prompt.
 */
void fun_SHELL_WRITE()
{
    const char *shellMsg = "Yaa Naaa3m: "; /* Prompt message */
    const char *shellMsg2 = " >>> "; /* Prompt suffix */
    int write_cheaker = 0; /* Variable to check if writing was successful */

    fun_PWD(); /* Call function to get current working directory */
    write(STDOUT, GREEN, strlen(GREEN)); /* Set text color to green */
    write_cheaker = write(STDOUT, shellMsg, strlen(shellMsg)); /* Write the prompt message */
    write(STDOUT, RESET, strlen(RESET)); /* Reset text color */
    if (write_cheaker <= 0) /* Check if writing was unsuccessful */
    {
        printf("Can't Read from user Shell will stop \n"); /* Print error message */
        perror("write"); /* Print system error message */
        exit(1); /* Exit with an error code */
    }

    write(STDOUT, BLUE, strlen(BLUE)); /* Set text color to blue */
    write_cheaker = write(STDOUT, pwd_PATH, strlen(pwd_PATH)); /* Write the current working directory */
    write(STDOUT, RESET, strlen(RESET)); /* Reset text color */
    if (write_cheaker <= 0) /* Check if writing was unsuccessful */
    {
        printf("Can't Read from user Shell will stop \n"); /* Print error message */
        perror("write"); /* Print system error message */
        exit(1); /* Exit with an error code */
    }

    write(STDOUT, RED, strlen(RED)); /* Set text color to red */
    write_cheaker = write(STDOUT, shellMsg2, strlen(shellMsg2)); /* Write the prompt suffix */
    write(STDOUT, RESET, strlen(RESET)); /* Reset text color */
    if (write_cheaker <= 0) /* Check if writing was unsuccessful */
    {
        printf("Can't Read from user Shell will stop \n"); /* Print error message */
        perror("write"); /* Print system error message */
        exit(1); /* Exit with an error code */
    }
}


/* Main function entry point */
int main(void)
{
    char  *token; /* Token for separating the command */
    char input_string_main_copy[INPUT_BUFFER]; /* Copy of input string to avoid modification by strtok in other functions */
    char envir_variable_checker[INPUT_BUFFER]; /* Copy of input string to check environment variables */

    int read_cheaker = 0; /* Variable to check if reading input was successful */

    while (1) /* Infinite loop for the shell to continuously accept commands */
    {
        fun_SHELL_WRITE(); /* Display the shell prompt */

        read_cheaker = read(STDIN, input_string, INPUT_BUFFER); /* Read input from the user */
        if (read_cheaker <= 0) /* Check if reading input was unsuccessful */
        {
            printf("Can't Read from user Shell will stop \n"); /* Print error message */
            perror("read"); /* Print system error message */
            break; /* Exit the loop */
        }

        REMOVE_LeadingSpaces(input_string); /* Remove leading spaces from input string */

        if (input_string[0] != '\n') { /* Check if input is not just a newline */
            remove_newline(input_string); /* Remove newline character from input string */
        }

        strcpy(input_string_main_copy, input_string); /* Copy input string to main copy for processing */
        strcpy(envir_variable_checker, input_string); /* Copy input string to check environment variables */

        char *set_check = strtok(envir_variable_checker, " "); /* Tokenize the input to check if it starts with "set" */
        if (strcmp(set_check, "set") == 0) /* Check if the command is "set" */
        {
            remove_set_prefix(input_string_main_copy); /* Remove "set" prefix from input string */
            char *equal_sign = strchr(input_string_main_copy, '='); /* Find the equal sign in the input string */
            if (equal_sign != NULL) /* Check if equal sign was found */
            {
                fun_PLACE_VARIABLE(input_string_main_copy, equal_sign, ENVIROMENT_VARIABLE); /* Place the environment variable */
                continue; /* Skip to the next iteration of the loop */
            }
            ERROR_MESSAGE(); /* Print error message */
            continue; /* Skip to the next iteration of the loop */
        }
        else
        {
            char *equal_sign = strchr(input_string_main_copy, '='); /* Find the equal sign in the input string */
            if (equal_sign != NULL) /* Check if equal sign was found */
            {
                fun_PLACE_VARIABLE(input_string_main_copy, equal_sign, LOCAL_VARIABLE); /* Place the local variable */
                continue; /* Skip to the next iteration of the loop */
            }
        }

        /* Get the first token (command) */
        token = strtok(input_string_main_copy, " ");

        if (*token == '\r' || *token == '\n') /* Check if token is just a carriage return or newline */
        {
            continue; /* Skip to the next iteration of the loop */
        }
        else if (is_internal_command(token)) /* Check if the token is an internal command */
        {
            if (strcmp(token, "help") == 0) /* Check if the command is "help" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    ERROR_MESSAGE(); /* Print error message */
                    continue; /* Skip to the next iteration of the loop */
                }
                fun_HELP(); /* Display help information */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "mypwd") == 0) /* Check if the command is "mypwd" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    ERROR_MESSAGE(); /* Print error message */
                    continue; /* Skip to the next iteration of the loop */
                }
                fun_PWD(); /* Display current working directory */
                printf("%s \n", pwd_PATH); /* Print the path */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "myecho") == 0) /* Check if the command is "myecho" */
            {
                fun_ECHO(); /* Display echoed text */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "exit") == 0) /* Check if the command is "exit" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    ERROR_MESSAGE(); /* Print error message */
                    continue; /* Skip to the next iteration of the loop */
                }
                printf(YELLOW "Hatw7shni y sa7by <_> \n5od balak 3la nfsk bye \n" RESET); /* Print farewell message */
                break; /* Exit the loop and terminate the shell */
            }
            else if (strcmp(token, "mycp") == 0) /* Check if the command is "mycp" */
            {
                fun_COPY(); /* Perform copy operation */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "mymv") == 0) /* Check if the command is "mymv" */
            {
                fun_MOVE(); /* Perform move operation */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "cd") == 0) /* Check if the command is "cd" */
            {
                fun_CD(); /* Change directory */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "type") == 0) /* Check if the command is "type" */
            {
                fun_TYPE(); /* Display type information */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "envir") == 0) /* Check if the command is "envir" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                fun_ENVIR(token); /* Display environment variables */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "phist") == 0) /* Check if the command is "phist" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    ERROR_MESSAGE(); /* Print error message */
                    continue; /* Skip to the next iteration of the loop */
                }
                fun_PHIST(); /* Print command history */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "free") == 0) /* Check if the command is "free" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    ERROR_MESSAGE(); /* Print error message */
                    continue; /* Skip to the next iteration of the loop */
                }
                fun_FREE(); /* Free memory or resources */
                continue; /* Skip to the next iteration of the loop */
            }
            else if (strcmp(token, "uptime") == 0) /* Check if the command is "uptime" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    ERROR_MESSAGE(); /* Print error message */
                    continue; /* Skip to the next iteration of the loop */
                }
                fun_UPTIME(); /* Display system uptime */
                continue; /* Skip to the next iteration of the loop */
            }   
            else if (strcmp(token, "allvars") == 0) /* Check if the command is "allvars" */
            {
                token = strtok(NULL, " "); /* Get the next token */
                if (token != NULL) /* Check if there are extra tokens */
                {
                    char *check_more = strtok(NULL, " "); /* Check for more tokens */
                    if (check_more != NULL) /* Check if there are more tokens */
                    {
                        ERROR_MESSAGE(); /* Print error message */
                        printf("allvars: prints all local and envir variables\nallvars -l: print local variables only\nenvir: prints enviroment variables only\n"); /* Print usage information */
                        continue; /* Skip to the next iteration of the loop */
                    }
                    fun_ALLVARS(token); /* Display all variables */
                    continue; /* Skip to the next iteration of the loop */
                }
                fun_ALLVARS(token); /* Display all variables */
                continue; /* Skip to the next iteration of the loop */
            }  
        }
        else if (is_external_command(token)) /* Check if the token is an external command */
        {
            fun_EXTERNAL_COMMAND(); /* Execute the external command */
            continue; /* Skip to the next iteration of the loop */
        }
        else
        {
            ERROR_MESSAGE(); /* Print error message for unknown commands */
        }

    }

    return 0; /* Return from main function */
}
  
