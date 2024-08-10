 /******************************************************************************
 *
 * File Name: internal_commands.h
 *
 * Description: Header file for internal_commands.c
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#ifndef INTERNAL_COMMANDS_H
#define INTERNAL_COMMANDS_H

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define INPUT_BUFFER        (100)
#define PWD_BUFFER          (100)   /*Used by pwd_PATH*/
#define READ_BUFFER         (1024)  /*Used in fun_COPY and fun_MOVE*/
#define MAX_LOCAL_VARIABLES (100)
#define ENVIROMENT_VARIABLE (1)
#define LOCAL_VARIABLE      (0)

typedef struct {
    long mem_total;
    long mem_free;
    long swap_total;
    long swap_free;
} MemoryInfo;

typedef struct {
    char *name;
    char *value;
} local_variable;

/*******************************************************************************
 *                           Extern Global Variable                            *
 *******************************************************************************/

extern char pwd_PATH[PWD_BUFFER]; /*Array of characters that store tha paths of the pwd*/

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/*
 * Description :
 * 1.Function to Add local or enviroment variable to the shell.
 */
void fun_PLACE_VARIABLE(char *input_string_main_copy,char *equal_sign,char VARIABLE_MODE);

/*
 * Description :
 * 1.Function to print env variables.
 */
void fun_ENVIR(char *token);

/*
 * Description :
 * 1.Function to print all local and enviroment variables.
 */
void fun_ALLVARS(char *which_variables);

/*
 * Description :
 * 1.Function to check the path /proc/uptime and print the uptime for the system and the total time spend in IDLE.
 */
void fun_UPTIME();

/*
 * Description :
 * 1.Function to check the path /proc/meminfo and extract Ram info and Swap area info
 * 2.Like Total meomory,free and used.
 */
void fun_FREE();

/*
 * Description :
 * 1.Function to check if \ is written.
 */
void fun_PATH_CHECKSPACES(char *PATH_SPACES);

/*
 * Description :
 * 1.Function to check if a command is internal it returns true
 */
int is_internal_command(const char *command);

/*
 * Description :
 * 1.Function to find it the command is Internal,external or not supported.
 */
void fun_TYPE();

/*
 * Description :
 * 1.Function to print the working directory path.
 */
void fun_PWD() ;

/*
 * Description :
 * 1.function to get help.
 */
void fun_HELP();

/*
 * Description :
 * 1.Function to print on the screen.
 */
void fun_ECHO();

/*
 * Description :
 * 1.Function to copy the Files from directory to another
 * 2.it can deal with spaces and can write the name of the file if the file doesn't write it
 * 3.it prints error if the file exist.
 */
void fun_COPY();

/*
 * Description :
 * 1.Function to move the Files from directory to another
 * 2.it can deal with spaces and can write the name of the file if the file doesn't write it
 * 3.it prints error if the file exist.
 */
void fun_MOVE();

/*
 * Description :
 * 1.Function to Change Directory*.
 * 2.It supports spaces in directories.
 */
void fun_CD();

#endif // INTERNAL_COMMANDS_H