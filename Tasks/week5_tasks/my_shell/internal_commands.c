/******************************************************************************
 *
 * File Name: internal_commands.c
 *
 * Description: Source file for the internal commands
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
#include "internal_commands.h"
#include "external_commands.h"  /* to use is_external_command function in fun_TYPE */

char pwd_PATH[PWD_BUFFER]; /* Array of characters that store the paths of the pwd */
extern char** environ; /* Get environ for env Function */
static const char *internal_commands[] = {"cd", "mypwd", "myecho", "help", "exit", "type", "mycp", "mymv", "envir", "phist", "free", "uptime", "allvars", NULL}; /* List of internal commands, PHIST is implemented in external_commands.c but it is an internal command */
static int exit_from_fun_PATH_CHECKSPACES = 1; /* To exit from fun_PATH_CHECKSPACES while loop */
static int return_variable = 0; /* Global variable that is used in fun_COPY and fun_MOVE to return from function fun_PATH_CONCATENATE if the filename ends with '\' */
static local_variable local_vars[MAX_LOCAL_VARIABLES];
static int local_var_count = 0;

/*
 * Description :
 * 1. Function to set local variables.
 */
static void SET_LOCAL_VARIABLE(char *name, char *value) 
{
    /* Iterate through existing local variables to check if the variable already exists */
    for (int i = 0; i < local_var_count; i++) {
        /* If variable name exists, update its value */
        if (strcmp(local_vars[i].name, name) == 0) {
            free(local_vars[i].value);
            local_vars[i].value = strdup(value);
            return;
        }
    }
    /* If variable doesn't exist and there's room, add new variable */
    if (local_var_count < MAX_LOCAL_VARIABLES) {
        local_vars[local_var_count].name = strdup(name);
        local_vars[local_var_count].value = strdup(value);
        local_var_count++;
    }
    else 
    {
        /* If the maximum number of local variables is reached, print an error */
        printf("Error You have reached the maximum number of local variables *_* \n");
    }
}

/*
 * Description :
 * 1. Function to print Error message if the user inputs a wrong format.
 */
static void ERROR_MESSAGE_FOR_LOCAL_VARIABLES()
{
    fflush(stdout); /* Flush the output buffer */
    printf("Please Enter a valid command -_- use help to know all commands \n");
    printf("When Trying to assign Envir use this format only\nset variable_name = variable value between two Qoutes\nNote:\n-You remove set to assign local variables\n-Variable name can't contain spaces\n");
}

/*
 * Description :
 * 1. Function to add a local variable to the shell.
 */
void fun_PLACE_VARIABLE(char *input_string_main_copy, char *equal_sign, char VARIABLE_MODE)
{
    /* First, check for quotes in the input string */
    char *first_quote_sign = strchr(input_string_main_copy, '"');
    if (first_quote_sign == NULL) 
    {
        /* If no quotes are found, display an error message */
        ERROR_MESSAGE_FOR_LOCAL_VARIABLES();
        return;
    }

    /* Error if the first quote is before the equal sign */
    if (first_quote_sign != NULL && first_quote_sign < equal_sign)
    {
        ERROR_MESSAGE_FOR_LOCAL_VARIABLES();
        return;
    }

    /* Check for the presence of the last quotes and make sure that it doesn't equal the position of the first quote */
    char *last_quote_sign = strrchr(input_string_main_copy, '"');
    if (last_quote_sign == NULL || (first_quote_sign == last_quote_sign)) 
    {
        ERROR_MESSAGE_FOR_LOCAL_VARIABLES();
        return;
    }

    *equal_sign = '\0'; /* Split the command into name and value */

    /* First part before '=' is the variable name */
    char *name = input_string_main_copy;
    name = strtok(name, " "); /* Remove spaces if the user put spaces between the variable name and '=' operation */
    char *check_name = strtok(NULL, " "); /* Check that no spaces are in the variable name */
    if (check_name != NULL) 
    {
        ERROR_MESSAGE_FOR_LOCAL_VARIABLES();
        return;
    }

    /* Remove quotes from value */
    first_quote_sign++; /* Move past the first quote */
    *last_quote_sign = '\0'; /* Null-terminate before the last quote */

    /* Check if it's an environment variable or a local variable */
    if (VARIABLE_MODE)
    {   
        /* Set the environment variable */
        if (setenv(name, first_quote_sign, 1) != 0)
        {
            perror("setenv"); /* Print an error if setenv fails */
        }
    }
    else
    {
        /* Set a local variable */
        SET_LOCAL_VARIABLE(name, first_quote_sign);
    }
}

/*
 * Description :
 * 1. Function to print environment variables.
 */
void fun_ENVIR(char *token)
{
    /* If no token is provided, print all environment variables */
    if (token == NULL) {
        int i = 0;
        while (environ[i] != NULL)
        {
            printf("%s\n", environ[i++]);
        }
    }
    else
    {
        char *check = strtok(NULL, " "); /* Check if there is any additional input after the token */
        if (check != NULL)
        {
            printf("Envir Variables Doesn't have spaces in the name, so try to find the correct name \n");
            return;
        }
        /* Get the environment variable value by name */
        char *variable = getenv(token);
        if (variable == NULL)
        {
            fflush(stdout); /* Flush the output buffer */
            printf("Sorry for you, Environment Doesn't have a variable = %s \n", token);
        }
        else 
        {
            printf("%s = %s \n", token, variable);
        }
    }
}

/*
 * Description :
 * 1. Function to print all local and environment variables.
 */
void fun_ALLVARS(char *which_variables)
{
    /* If no argument is passed, print both local and environment variables */
    if (which_variables == NULL) {
        /* Print local variables */
        printf("------- Local Variables ------- \n");
        for (int i = 0; i < local_var_count; ++i) {
            printf("%s=%s\n", local_vars[i].name, local_vars[i].value);
        }

        /* Print environment variables */
        printf("\n------- ENVIR Variables ------- \n");
        fun_ENVIR(NULL);
    } 
    else if (strcmp(which_variables, "-l") == 0) {
        /* Print local variables only if "-l" argument is passed */
        printf("------- Local Variables ------- \n");
        for (int i = 0; i < local_var_count; ++i) {
            printf("%s=%s\n", local_vars[i].name, local_vars[i].value);
        }
    } 
    else {
        /* Print an error message if an invalid argument is passed */
        printf("Please Enter a valid command -_- use help to know all commands \n");
        printf("allvars: prints all local and envir variables\nallvars -l: print local variables only\n");
        printf("envir: prints environment variables only\n");
    }
}

/*
 * Description :
 * 1. Function to check the path /proc/uptime and print the uptime for the system and the total time spent in IDLE.
 */
void fun_UPTIME()
{
    /* Open the /proc/uptime file */
    FILE *file = fopen("/proc/uptime", "r");
    if (!file) {
        perror("fopen"); /* Print an error if the file cannot be opened */
        return;
    }

    double uptime, idle;
    /* Read the uptime and idle time from the file */
    if (fscanf(file, "%lf %lf", &uptime, &idle) != 2) {
        perror("fscanf"); /* Print an error if reading fails */
        fclose(file);
        return;
    }
    fclose(file); /* Close the file */

    /* Calculate uptime in days, hours, and minutes */
    int up_days = uptime / (60 * 60 * 24);
    int up_hours = (uptime / (60 * 60)) - (up_days * 24);
    int up_minutes = (uptime / 60) - (up_days * 24 * 60) - (up_hours * 60);

    /* Print the uptime and idle time */
    printf("Uptime: %d days, %d hours, %d minutes\n", up_days, up_hours, up_minutes);
    printf("Idle time: %.2f seconds\n", idle);
}

/*
 * Description :
 * 1. Function to check the path /proc/meminfo and extract RAM info and Swap area info.
 * 2. Like Total memory, free, and used.
 */
void fun_FREE()
{
    /* Open the /proc/meminfo file */
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        perror("fopen"); /* Print an error if the file cannot be opened */
        return;
    }

    char line[256];
    MemoryInfo info = {0}; /* Initialize memory info structure to zero */

    /* Read lines from the file and extract memory information */
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %ld kB", &info.mem_total) == 1 ||
            sscanf(line, "MemFree: %ld kB", &info.mem_free) == 1 ||
            sscanf(line, "SwapTotal: %ld kB", &info.swap_total) == 1 ||
            sscanf(line, "SwapFree: %ld kB", &info.swap_free) == 1) {
            continue;
        }
    }

    fclose(file); /* Close the file */

    /* Print RAM information */
    printf("RAM Info:\n");
    printf("  Total:    %ld kB\n", info.mem_total);
    printf("  Used:     %ld kB\n", info.mem_total - info.mem_free);
    printf("  Free:     %ld kB\n", info.mem_free);

    /* Print Swap information */
    printf("Swap Info:\n");
    printf("  Total:    %ld kB\n", info.swap_total);
    printf("  Used:     %ld kB\n", info.swap_total - info.swap_free);
    printf("  Free:     %ld kB\n", info.swap_free);
}

/*
 * Description :
 * 1.Function to concatenate the string after \ with its space to be used in copy, move, and cd functions.
 */
static void fun_PATH_CONCATENATE(char *PATH1)
{
    /* Get the next token to concatenate */
    char *PATH2 = strtok(NULL, " ");

    /* If no token is found, exit */
    if (PATH2 == NULL) {
        exit_from_fun_PATH_CHECKSPACES = 1;
        return;
    }

    /* Check if the next token starts with '/', which is not allowed */
    if (PATH2[0] == '/') {
        exit_from_fun_PATH_CHECKSPACES = 1;
        printf("File names ending with a backslash can't be moved or copied. Change the name and try again ^-^ \n");
        return_variable = 1;
        return;
    }

    int len1 = strlen(PATH1);
    int len2 = strlen(PATH2);

    /* Calculate the new length for concatenation */
    size_t new_length = len1 + len2 + 2; // +1 for space, +1 for null terminator

    /* Allocate memory for the new concatenated path */
    char *new_path = (char *)malloc(new_length);
    if (new_path == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    /* Copy PATH1 to new_path, excluding the last '\' */
    strncpy(new_path, PATH1, len1 - 1);
    new_path[len1 - 1] = '\0';

    /* Concatenate a space and PATH2 */
    strcat(new_path, " ");
    strcat(new_path, PATH2);

    /* Update PATH1 with the new concatenated path */
    strcpy(PATH1, new_path);

    /* Free the allocated memory */
    free(new_path);
}

/*
 * Description :
 * 1.Function to check if '\' is written.
 */
void fun_PATH_CHECKSPACES(char *PATH_SPACES)
{
    /* Get the length of the input path */
    int len1 = strlen(PATH_SPACES);
    exit_from_fun_PATH_CHECKSPACES = 0;

    /* If the last character is '\', concatenate the next token */
    while ((PATH_SPACES[len1 - 1] == '\\') && (exit_from_fun_PATH_CHECKSPACES == 0)) {
        fun_PATH_CONCATENATE(PATH_SPACES);
        len1 = strlen(PATH_SPACES);
    }
}

/*
 * Description :
 * 1.Function to check if a command is internal; it returns true.
 */
int is_internal_command(const char *command)
{
    int i = 0;

    /* Loop through the list of internal commands to check if the given command matches */
    while (internal_commands[i] != NULL) {
        if (strcmp(command, internal_commands[i]) == 0) {
            return 1;
        }
        i++;
    }
    return 0; /* Return false if the command is not found in the list */
}

/*
 * Description :
 * 1.Function to determine if the command is Internal, External, or Unsupported.
 */
void fun_TYPE()
{
    /* Buffer to store the command to be checked */
    char command_buffer[20];
    int flag = 0;

    /* Extract the command to be checked from the input */
    char *type_command = strtok(NULL, " ");
    if (type_command == NULL) {
        printf("Error: Command is missing.\n");
        return;
    }

    /* Check if there is an additional argument after the command */
    char *type_check = strtok(NULL, " ");
    if (type_check != NULL) {
        printf("Command is an unsupported command.\n");
        return;
    }

    /* If the command starts with '/', it's considered unsupported */
    if (type_command[0] == '/') {
        printf("Command is an unsupported command. Please provide the program name, not the path. \n");
        return;
    }

    /* Check if the command is internal, external, or unsupported */
    if (is_internal_command(type_command)) {
        printf("Command '%s' is an internal command.\n", type_command);
    } else if (is_external_command(type_command)) {
        printf("Command '%s' is an external command.\n", type_command);
    } else {
        printf("Command is an unsupported command.\n");
    }
}

/*
 * Description :
 * 1.Function to print the working directory path.
 */
void fun_PWD() 
{
    /* Flush the output buffer to ensure all output is written */
    fflush(stdout);
    
    /* Get the current working directory and store it in pwd_PATH */
    getcwd(pwd_PATH, PWD_BUFFER);
}   

/*
 * Description :
 * 1.Function to get help.
 */
void fun_HELP()
{
    /* Flush the output buffer to ensure all output is written */
    fflush(stdout);
    
    /* Print the list of supported commands */
    printf("Supported Commands: \n");
    printf(" 1) mypwd: to print the working directory \n 2) myecho: to print the text \n");
    printf(" 3) mycp: to copy the files from path to another \n 4) mymv: to move the files from path to another \n 5) help: to get all the commands supported \n");
    printf(" 6) cd: to change the working directory\n 7) envir: to print All Enviroment variables \n");
    printf(" 8) envir variable: to print value of enviroment variable if exist \n 9) type: to find if the command is Internal external or not supported\n");
    printf(" 10) phist: list the last 10 processes with their exit status \n 11) free: list Ram info and Swap Area info\n 12) uptime: it checks the uptime for the system and the time spent in IDLE.\n");
    printf(" 13) set VAR_ENVIR=karim (between two quotes): assign envir variable, VAR_LOCAL=karim (between two quotes): assign local variable\n 14) allvars: Print All local and environment Variables, you can use allvars -l to print local variables only\n");
    printf(" 15) Shell supports all external commands found in environment variable locations\n 16) exit: to terminate the program\n");
}

/*
 * Description :
 * 1.Function to print on the screen.
 */
void fun_ECHO()
{
    /* Flush the output buffer to ensure all output is written */
    fflush(stdout);
    
    /* Extract the text to echo from the command input */
    char* echo_variable = strtok(NULL, " ");

    /* Check if the echo text starts with a double quote */
    if (echo_variable != NULL && echo_variable[0] == '"') {
        echo_variable++;  /* Skip the initial double quote */

        /* Loop through the remaining tokens to print them */
        while (echo_variable != NULL) {
            size_t len = strlen(echo_variable);

            /* Check if the last character is a double quote and it's not the first character */
            if (len > 0 && echo_variable[len - 1] == '"') {
                echo_variable[len - 1] = '\0';  /* Remove the ending double quote */
                printf("%s", echo_variable);    /* Print the final token without trailing space */
                echo_variable = strtok(NULL, " ");

                /* Print an error if there are extra tokens outside of quotes */
                if (echo_variable != NULL) {
                    printf("\nError in myecho: If you used quotes, make sure all the words are inside.\n");
                    return;
                }
            } else {
                /* Print the current token with a trailing space */
                printf("%s ", echo_variable);
            }

            echo_variable = strtok(NULL, " ");
        }
    } else {
        /* Print tokens if no double quotes are used */
        while (echo_variable != NULL) {
            printf("%s ", echo_variable);
            echo_variable = strtok(NULL, " ");
        }
    }

    /* Print a newline after echoing the text */
    printf("\n");
    
    /* Flush the output buffer to ensure all output is written */
    fflush(stdout);
}

/*
 * Description :
 * 1.Function to copy the Files from directory to another.
 * 2.It can deal with spaces and can write the name of the file if the file doesn't write it.
 * 3.It prints an error if the file exists.
 */
void fun_COPY()
{
    /* Extract the source path from the command input */
    char *sourcePath = strtok(NULL, " ");
    
    /* Check if the source path is provided */
    if (sourcePath == NULL) {
        printf("Error: Source path is missing.\n");
        return;
    }

    /* Check for spaces in the source path */
    fun_PATH_CHECKSPACES(sourcePath);
    
    /* If there was an issue with spaces in the path, return */
    if (return_variable) {
        return_variable = 0; /* Reset the return variable */
        return;
    }

    /* Open the source file for reading */
    int fd_src = open(sourcePath, O_RDONLY);
    
    /* Check if the source file was successfully opened */
    if (fd_src == -1) {
        printf("Error opening source file \n"); 
        return;
    }

    /* Extract the target path from the command input */
    char *targetPath = strtok(NULL, " ");
    
    /* Check if the target path is provided */
    if (targetPath == NULL) {
        printf("Error: Target path is missing.\n");
        close(fd_src); /* Close the source file descriptor */
        return;
    }

    /* Check for spaces in the target path */
    fun_PATH_CHECKSPACES(targetPath);

    /* Checking if the target file exists or if the user provided a directory, 
     * and if so, name the file by its original name */
    struct stat st;
    if (stat(targetPath, &st) == 0 && S_ISDIR(st.st_mode)) {
        /* The target path is a directory; construct the target file name */
        char *sourceFileName = strrchr(sourcePath, '/');
        sourceFileName = (sourceFileName == NULL) ? sourcePath : sourceFileName + 1;

        /* Construct the full target path with the source file name */
        char fullTargetPath[INPUT_BUFFER];
        snprintf(fullTargetPath, sizeof(fullTargetPath), "%s/%s", targetPath, sourceFileName);

        /* Check if the constructed target file already exists */
        if (access(fullTargetPath, F_OK) == 0) {
            printf("Error: Target file '%s' already exists.\n", fullTargetPath);
            close(fd_src); /* Close the source file descriptor */
            return;
        }

        /* Set targetPath to the constructed full target path */
        targetPath = fullTargetPath;
    } else {
        /* Check if the target file already exists */
        if (access(targetPath, F_OK) == 0) {
            printf("Error: Target file '%s' already exists.\n", targetPath);
            close(fd_src); /* Close the source file descriptor */
            return;
        }
    }

    /* Open the target file for writing, creating it if necessary, and truncating it if it exists */
    int fd_dest = open(targetPath, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    
    /* Check if the target file was successfully opened */
    if (fd_dest == -1) {
        /* Flush the output buffer to ensure all output is written */
        fflush(stdout);
        
        /* Print an error message if the target file couldn't be opened */
        printf("Error in destination file, make sure you entered the full path correctly ^_^ \n"); 
        return;
    }

    /* Buffer to hold data read from the source file */
    char buff[READ_BUFFER];
    int size = 0;

    /* Copy the file content from the source to the destination */
    while ((size = read(fd_src, buff, sizeof(buff))) > 0) {
        if (write(fd_dest, buff, size) != size) {
            printf("Error writing to destination file \n");
            close(fd_src); /* Close the source file descriptor */
            close(fd_dest); /* Close the destination file descriptor */
            return;
        }
    }

    /* Close the file descriptors after copying */
    close(fd_src);
    close(fd_dest);

    /* Flush the output buffer to ensure all output is written */
    fflush(stdout);
    
    /* Print a success message after the file is successfully copied */
    printf("File Copied successfully ^-^ \n");
}

/*
 * Description :
 * 1.Function to move the Files from directory to another
 * 2.it can deal with spaces and can write the name of the file if the file doesn't write it
 * 3.it prints error if the file exist.
 */
void fun_MOVE()
{
    /* Extract the source path from the command input */
    char *sourcePath = strtok(NULL, " ");
    
    /* Check if the source path is provided */
    if (sourcePath == NULL) {
        /* Print an error message if the source path is missing */
        printf("Error: Source path is missing.\n");
        return;
    }

    /* Check for spaces in the source path */
    fun_PATH_CHECKSPACES(sourcePath);
    
    /* If there was an issue with spaces in the path, return */
    if (return_variable) {
        return_variable = 0; /* Reset the return variable */
        return;
    }

    /* Open the source file for reading */
    int fd_src = open(sourcePath, O_RDONLY);
    
    /* Check if the source file was successfully opened */
    if (fd_src == -1) {
        /* Print an error message if the source file couldn't be opened */
        printf("Error opening source file \n");
        return;
    }

    /* Extract the target path from the command input */
    char *targetPath = strtok(NULL, " ");
    
    /* Check if the target path is provided */
    if (targetPath == NULL) {
        /* Print an error message if the target path is missing */
        printf("Error: Target path is missing.\n");
        close(fd_src); /* Close the source file descriptor */
        return;
    }

    /* Check for spaces in the target path */
    fun_PATH_CHECKSPACES(targetPath);

    /* Check if the target file exists or if the user provided a directory, 
     * and if so, name the file by its original name */
    struct stat st;
    if (stat(targetPath, &st) == 0 && S_ISDIR(st.st_mode)) {
        /* The target path is a directory; construct the target file name */
        char *sourceFileName = strrchr(sourcePath, '/');
        sourceFileName = (sourceFileName == NULL) ? sourcePath : sourceFileName + 1;

        /* Construct the full target path with the source file name */
        char fullTargetPath[INPUT_BUFFER];
        snprintf(fullTargetPath, sizeof(fullTargetPath), "%s/%s", targetPath, sourceFileName);

        /* Check if the constructed target file already exists */
        if (access(fullTargetPath, F_OK) == 0) {
            /* Print an error message if the target file already exists */
            printf("Error: Target file '%s' already exists.\n", fullTargetPath);
            close(fd_src); /* Close the source file descriptor */
            return;
        }

        /* Set targetPath to the constructed full target path */
        targetPath = fullTargetPath;
    } else {
        /* Check if the target file already exists */
        if (access(targetPath, F_OK) == 0) {
            /* Print an error message if the target file already exists */
            printf("Error: Target file '%s' already exists.\n", targetPath);
            close(fd_src); /* Close the source file descriptor */
            return;
        }
    }

    /* Open the target file for writing, creating it if necessary, and truncating it if it exists */
    int fd_dest = open(targetPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    /* Check if the target file was successfully opened */
    if (fd_dest == -1) {
        /* Print an error message if the target file couldn't be opened */
        printf("Error opening destination file\n ");
        close(fd_src); /* Close the source file descriptor */
        return;
    }

    /* Buffer to hold data read from the source file */
    char buff[READ_BUFFER];
    int size;

    /* Copy the file content from the source to the destination */
    while ((size = read(fd_src, buff, sizeof(buff))) > 0) {
        /* Write the data to the destination file */
        if (write(fd_dest, buff, size) != size) {
            /* Print an error message if writing to the destination file failed */
            printf("Error writing to destination file \n");
            close(fd_src); /* Close the source file descriptor */
            close(fd_dest); /* Close the destination file descriptor */
            return;
        }
    }

    /* Close the file descriptors after copying */
    close(fd_src);
    close(fd_dest);

    /* Remove the source file after copying */
    if (remove(sourcePath) != 0) {
        /* Print an error message if deleting the source file failed */
        printf("Error deleting source file \n");
    } else {
        /* Print a success message after the file is successfully moved */
        printf("File moved successfully.\n");
    }
}

/* 
 * Description:
 * 1. Function to Change Directory.
 * 2. It supports spaces in directories.
 */
void fun_CD()
{
    /* Extract the path to change to from the command input */
    char *changePath = strtok(NULL, " ");
    
    /* Check if the change path is provided */
    if (changePath == NULL) {
        /* Print an error message if the change path is missing */
        printf("Error: Change path is missing.\n");
        return;
    }

    /* Check for spaces in the change path */
    fun_PATH_CHECKSPACES(changePath);

    /* Attempt to change the directory */
    int status = chdir(changePath);
    
    /* Check if the directory change was successful */
    if (status == 0) {
        /* Print a success message */
        printf("Directory Changed ^_^ \n");
    } else {
        /* Print an error message if the directory change failed */
        perror("Error Changing Directory because ");
        printf("\n");
    }
}
