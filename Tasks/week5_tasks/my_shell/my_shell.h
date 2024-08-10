 /******************************************************************************
 *
 * File Name: my_shell.h
 *
 * Description: Header file for shell_v3.c
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#ifndef SHELL_MAIN_H
#define SHELL_MAIN_H

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define STDIN               (0)
#define STDOUT              (1)
#define STDERR              (2)
#define INPUT_BUFFER        (100)
#define ENVIROMENT_VARIABLE (1)
#define LOCAL_VARIABLE      (0)

// Define ANSI color codes
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[37m"

/*******************************************************************************
 *                           Extern Global Variable                            *
 *******************************************************************************/

extern char input_string[INPUT_BUFFER]; /* Global function to get the command and use in all functions*/

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/*
 * Description :
 * 1. Function to remove the Leading spaces if the input_string contain.
 */
void REMOVE_LeadingSpaces(char *str);

/*
 * Description :
 * 1. Function to remove the new line in input_string.
 */
void remove_newline(char *str);

/*
 * Description :
 * 1. Function to Write on the Shell.
 */
void fun_SHELL_WRITE();


#endif // SHELL_MAIN__H