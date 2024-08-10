 /******************************************************************************
 *
 * File Name: external_commands.h
 *
 * Description: Header file for external_commands.c
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#ifndef EXTERNAL_COMMANDS_H
#define EXTERNAL_COMMANDS_H

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define INPUT_BUFFER        (100)
#define MAX_HISTORY         (10)
#define MAX_PROCESS_NAME    (50)
#define OUTPUT_REDIRECTION  (0)
#define INPUT_REDIRECTION   (0)
#define ERROR_REDIRECTION   (1)
#define STD_IN              (0)
#define STD_OUT             (1)
#define STD_ERROR           (2)

/*Struct to save Process pids and exit status*/
typedef struct {
    int pid;
    int exit_status;
    char process_name[MAX_PROCESS_NAME];
} ProcessInfo;

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/*
 * Description :
 * 1. Function to be called to display the last 10 process occured with there pid and exit status and its name ,  
 * 2. It is an internal command it was implemented here as it used to display external commands processes.
 */
void fun_PHIST();

/*
 * Description :
 * 1. Function to check if a command is external or not.
 */
int is_external_command(const char *command);

/*
 * Description :
 * 1. Function to be called if is_external_command is true .
 * 2. you can call external commands by its name or its paths.
 */
void fun_EXTERNAL_COMMAND();

#endif // EXTERNAL_COMMANDS_H