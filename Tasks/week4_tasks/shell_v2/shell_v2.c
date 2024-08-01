/*******************************************************************************************************
 * Name: shell_v2.c
 *
 * Description:Shell Program used to do the Following commands (mypwd,myecho,mycp,mymv,help,type,envir,envir variable,cd,exit and all external commands)
 *
 * Author: Karim Gomaa
 ********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define STDIN           (0)
#define STDOUT          (1)
#define STDERR          (2)
#define INPUT_BUFFER   (100)
#define READ_BUFFER    (1024)
#define PWD_BUFFER     (100)
#define MAX_HISTORY    (10)

char input_string[INPUT_BUFFER]; /* Global function to get the command and use in all functions*/
char pwd_PATH[PWD_BUFFER];
extern char** environ; /*Get environ for env Function*/
const char *internal_commands[] = {"cd", "mypwd", "myecho", "help", "exit","type", "mycp" , "mymv" , "envir", "envir variable","phist", NULL}; /* List of internal commands */
int exit_from_fun_PATH_CHECKSPACES = 1; /*To exit form fun_PATH_CHECKSPACES while loop*/
int return_variable=0;

/*Struct to save Process pids and exit status*/
typedef struct {
    pid_t pid;
    int exit_status;
} ProcessInfo;

ProcessInfo process_history[MAX_HISTORY]; /*Global Array of Variable of Struct that has Max history defined number */
int process_count = 0; /*start count with 0*/

/*Function to add a process in the Array process_history that we created*/
void add_process(pid_t pid, int exit_status) {
    if (process_count < MAX_HISTORY) {
        process_history[process_count].pid = pid;
        process_history[process_count].exit_status = exit_status;
        process_count++;
    } else {
        // Shift history to make room for new entry
        memmove(process_history, process_history + 1, (MAX_HISTORY - 1) * sizeof(ProcessInfo));
        process_history[MAX_HISTORY - 1].pid = pid;
        process_history[MAX_HISTORY - 1].exit_status = exit_status;
    }
}

/*Function to be called to display the last 10 process occured with there pid and exit status*/
void fun_PHIST() {
    printf("Last %d processes:\n", process_count);
    printf("0 means: succes , Other Numbers means: error \n");
    printf("----------------------------------------------\n");
    for (int i = 0; i < process_count; i++) {
        printf("PID: %d, Exit Status: %d\n", process_history[i].pid, process_history[i].exit_status);
    }
}

// Function to concatenate if there is a '\'
void fun_PATH_CONCATENATE(char *PATH1) {
    char *PATH2 = strtok(NULL, " ");

    if(PATH2 == NULL)
    {
        exit_from_fun_PATH_CHECKSPACES=1;
        return;
    }

    if(PATH2[0] == '/')
    {
        exit_from_fun_PATH_CHECKSPACES=1;
        printf("File names end with backslash can't be moved or copied change the name and try agian ^-^ \n");
        return_variable=1;
        return;
    }

    int len1 = strlen(PATH1);
    int len2 = strlen(PATH2);

    // Calculate the new length
    size_t new_length = len1 + len2 + 2; // +1 for space, +1 for null terminator


    // Allocate new memory to accommodate the concatenated string
    char *new_path = (char *)malloc(new_length);
    if (new_path == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Copy PATH1 to the new_path, removing the last '\'
    strncpy(new_path, PATH1, len1 - 1);
    new_path[len1 - 1] = '\0';


    // Concatenate a space and PATH2
    strcat(new_path, " ");
    strcat(new_path, PATH2);

    // Update PATH1 with the new_path value
    strcpy(PATH1, new_path);

    // Free the allocated memory
    free(new_path);
}

/*Function to check if \ is written*/
void fun_PATH_CHECKSPACES(char *PATH_SPACES){

    // Get the length of token1
    int len1 = strlen(PATH_SPACES);
    exit_from_fun_PATH_CHECKSPACES=0;
    // Check if the last character of token1 is '\'
    while( (PATH_SPACES[len1 - 1] == '\\' ) && (exit_from_fun_PATH_CHECKSPACES == 0) )
    {
        fun_PATH_CONCATENATE(PATH_SPACES);
        len1 = strlen(PATH_SPACES);
    }
}

// Function to check if a command is internal
int is_internal_command(const char *command) {
    int i = 0;
    while (internal_commands[i] != NULL) {
        if (strcmp(command, internal_commands[i]) == 0) {
            return 1;
        }
        i++;
    }
    return 0;
}

// Function to check if a command is external
int is_external_command(const char *command) {
    char *path = getenv("PATH");
    char *token;
    char full_path[512];

    /*Checking it the user send a full path*/
    if(command[0] == '/')
    {
        if (access(command, X_OK) == 0) {
            return 1; // Command is executable
        }
    }

    if (path == NULL) {
        return 0; // No PATH environment variable found
    }

    // Duplicate the PATH string so we can use strtok without affecting the original
    char *path_dup = strdup(path);
    token = strtok(path_dup, ":");

    while (token != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
        if (access(full_path, X_OK) == 0) {
            free(path_dup);
            return 1; // Command is executable
        }
        token = strtok(NULL, ":");
    }

    free(path_dup);
    return 0; // Command not found in any PATH directory
}

/*Function to find it the command is Internal external or not supported*/
void fun_TYPE()
{
    char command_buffer[20];
    int flag=0;

    char *type_command = strtok(NULL, " ");
    if (type_command == NULL) {
        printf("Error: Command is missing.\n");
        return;
    }

    char *type_check=strtok(NULL, " ");
    if( type_check != NULL )
    {
        if(strcmp(type_command,"envir") == 0 && strcmp(type_check,"variable") == 0)
        {
            char *type_check2=strtok(NULL, " ");
            if(type_check2 != NULL)
            {
                fflush(stdout);
                printf("Command is an unsupported command.\n");
                return;
            }
            else
            {
                snprintf(command_buffer, sizeof(command_buffer), "%s %s", type_command, type_check);
                flag=1;
            }
        }else 
        {
            printf("Command is an unsupported command.\n");
            return;
        }

    }
    else
    {
        if(type_command[0] == '/')
        {
            printf("Command is an unsupported command. Please provide the program name not path. \n");
            return;
        }

    }



    if(!flag)
    {
        if (is_internal_command(type_command))
        {
            printf("Command '%s' is an internal command.\n", type_command);
        }
        else if (is_external_command(type_command)) 
        {
            printf("Command '%s' is an external command.\n", type_command);
        } 
        else
        {
            printf("Command is an unsupported command.\n");
        }      
    }
    else
    {
        if (is_internal_command(command_buffer))
        {
            printf("Command '%s' is an internal command.\n", command_buffer);
        }
        else if (is_external_command(command_buffer)) 
        {
            printf("Command '%s' is an external command.\n", command_buffer);
        } 
        else
        {
            printf("Command is an unsupported command.\n");
        }
    }
}

/*Function to remove newlines from the token given by the user.*/
void remove_newline(char *str) {
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) {
        *pos = '\0';
    }
}

/*Function to remove the leading spaces*/
void REMOVE_LeadingSpaces(char *str) {
    int i = 0;
    int j = 0;

    // Find the first non-space character
    while (str[i] == ' ') {
        i++;
    }

    // Shift the characters to the left
    while (str[i]) {
        str[j++] = str[i++];
    }

    // Null-terminate the string
    str[j] = '\0';
}

/* Function to print the working directory path */
void fun_PWD() 
{
    fflush(stdout);
    getcwd(pwd_PATH,PWD_BUFFER);
}   

/*function to get help */
void fun_HELP()
{
    fflush(stdout);
    printf("Supported Commands: \n");
    printf(" 1) mypwd: to print the working directory \n 2) myecho: to print the text \n");
    printf(" 3) mycp: to copy the files from path to another \n 4) mymv: to move the files from path to another \n 5) help: to get all the commands suppurted \n");
    printf(" 6) cd: to change the working directory\n 7) envir: to print All Enviroment variables \n");
    printf(" 8) envir variable: to print value of enviroment variable if exist \n 9) type: to find it the command is Internal external or not supported\n");
    printf(" 10) phist: list the last 10 processes with there exit status \n 11) Shell supports all external commands found in enviroment variable locations\n 12) exit: to terminate the program\n");
}

/* Function to print on the screen */
void fun_ECHO() {
    fflush(stdout);
    char* echo_variable = strtok(NULL, " ");

    if (echo_variable != NULL && echo_variable[0] == '"') {
        echo_variable++;  // Skip the initial double quote

        while (echo_variable != NULL) {
            size_t len = strlen(echo_variable);
            
            // Check if the last character is a double quote and it's not the first character
            if (len > 0 && echo_variable[len - 1] == '"') {
                echo_variable[len - 1] = '\0';  // Remove the ending double quote
                printf("%s", echo_variable);    // Print the final token without trailing space
                echo_variable = strtok(NULL, " ");
                
                if (echo_variable != NULL) {
                    printf("\nError in myecho: If you used quotes, make sure all the words are inside.\n");
                    return;
                }
            } else {
                printf("%s ", echo_variable);
            }

            echo_variable = strtok(NULL, " ");
        }
    } else {
        while (echo_variable != NULL) {
            printf("%s ", echo_variable);
            echo_variable = strtok(NULL, " ");
        }
    }

    printf("\n");
    fflush(stdout);
}

/*Function to copy files */
void fun_COPY()
{
    char *sourcePath = strtok(NULL, " ");
    if (sourcePath == NULL) {
        printf("Error: Source path is missing.\n");
        return;
    }

    fun_PATH_CHECKSPACES(sourcePath);
    if(return_variable)
    {
        return_variable=0;
        return;
    }

    int fd_src = open(sourcePath, O_RDONLY);
    if (fd_src == -1) {
        printf("Error opening source file \n"); 
        return;
    }

    char *targetPath = strtok(NULL, " ");
    if (targetPath == NULL) {
        printf("Error: Target path is missing.\n");
        close(fd_src);
        return;
    }

    fun_PATH_CHECKSPACES(targetPath);

    /* Checking if the Target file is exist or if the user didn't send the name of the so we name the file by it's name */
    struct stat st;
    if (stat(targetPath, &st) == 0 && S_ISDIR(st.st_mode)) {
        // Target path is a directory; construct target file name
        char *sourceFileName = strrchr(sourcePath, '/');
        sourceFileName = (sourceFileName == NULL) ? sourcePath : sourceFileName + 1;

        // Construct the full target path with the source file name
        char fullTargetPath[INPUT_BUFFER];
        snprintf(fullTargetPath, sizeof(fullTargetPath), "%s/%s", targetPath, sourceFileName);

        // Check if the constructed target file already exists
        if (access(fullTargetPath, F_OK) == 0) {
            printf("Error: Target file '%s' already exists.\n", fullTargetPath);
            close(fd_src);
            return;
        }

        targetPath = fullTargetPath;
    } else {
        // Check if the target file already exists
        if (access(targetPath, F_OK) == 0) {
            printf("Error: Target file '%s' already exists.\n", targetPath);
            close(fd_src);
            return;
        }
    }

    int fd_dest = open(targetPath,O_WRONLY | O_CREAT | O_TRUNC , 0644); 
    if (fd_dest == -1)
    {
        fflush(stdout);
        printf( "Error in destenation file, make sure you entered the full path correct ^_^ \n"); 
        return ;
    }

    char buff[READ_BUFFER];
    int size=0;

    // Copy the file content from source to destination
    while ((size = read(fd_src, buff, sizeof(buff))) > 0) {
        if (write(fd_dest, buff, size) != size) {
            printf("Error writing to destination file \n");
            close(fd_src);
            close(fd_dest);
            return;
        }
    }

    close(fd_src);
    close(fd_dest);

    fflush(stdout);
    printf("File Copied successfully ^-^ \n");
}

/*Function to move the Files*/
void fun_MOVE(){
    char *sourcePath = strtok(NULL, " ");
    if (sourcePath == NULL) {
        printf("Error: Source path is missing.\n");
        return;
    }

    fun_PATH_CHECKSPACES(sourcePath);
    if(return_variable)
    {
        return_variable=0;
        return;
    }

    int fd_src = open(sourcePath, O_RDONLY);
    if (fd_src == -1) {
        printf("Error opening source file \n");
        return;
    }

    char *targetPath = strtok(NULL, " ");
    if (targetPath == NULL) {
        printf("Error: Target path is missing.\n");
        close(fd_src);
        return;
    }

    fun_PATH_CHECKSPACES(targetPath);

    /* Checking if the Target file exists or if the user didn't send the name of the so we name the file by its name */
    struct stat st;
    if (stat(targetPath, &st) == 0 && S_ISDIR(st.st_mode)) {
        // Target path is a directory; construct target file name
        char *sourceFileName = strrchr(sourcePath, '/');
        sourceFileName = (sourceFileName == NULL) ? sourcePath : sourceFileName + 1;

        // Construct the full target path with the source file name
        char fullTargetPath[INPUT_BUFFER];
        snprintf(fullTargetPath, sizeof(fullTargetPath), "%s/%s", targetPath, sourceFileName);

        // Check if the constructed target file already exists
        if (access(fullTargetPath, F_OK) == 0) {
            printf("Error: Target file '%s' already exists.\n", fullTargetPath);
            close(fd_src);
            return;
        }

        targetPath = fullTargetPath;
    } else {
        // Check if the target file already exists
        if (access(targetPath, F_OK) == 0) {
            printf("Error: Target file '%s' already exists.\n", targetPath);
            close(fd_src);
            return;
        }
    }

    int fd_dest = open(targetPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        printf("Error opening destination file\n ");
        close(fd_src);
        return;
    }

    char buff[READ_BUFFER];
    int size;

    // Copy the file content from source to destination
    while ((size = read(fd_src, buff, sizeof(buff))) > 0) {
        if (write(fd_dest, buff, size) != size) {
            printf("Error writing to destination file \n");
            close(fd_src);
            close(fd_dest);
            return;
        }
    }

    // Close file descriptors
    close(fd_src);
    close(fd_dest);

    // Remove the source file after copying
    if (remove(sourcePath) != 0) {
        printf("Error deleting source file \n");
    } else {
        printf("File moved successfully.\n");
    }
}

/*Function to Change Directory*/
void fun_CD(){


    char *changePath = strtok(NULL, " ");
    if (changePath == NULL) {
        printf("Error: Change path is missing.\n");
        return;
    }

    fun_PATH_CHECKSPACES(changePath);

    int status=chdir(changePath);
    if (status == 0)
    {
        printf("Directory Changed to ^_^ \n");
    }
    else
    {
        perror("Error Changing Directory because ");
        printf("\n");
    }

}

/* Function to print env variables */
void fun_ENVIR(){

    int i =0 ;
    while(environ[i] !=NULL)
    {
        printf("%s\n", environ[i++]);
    }

}

void fun_ENVIR_VARIABLE()
{
    char *path = getenv("PATH");
    printf("PATH =%s \n",path);
}

void fun_EXTERNAL_COMMAND()
{

    /*to be used in strtok as strtok changes the string so we need the input to be not changed in extrnal command function*/
    char input_string_func_copy1[INPUT_BUFFER];
    char input_string_func_copy2[INPUT_BUFFER];
    strcpy(input_string_func_copy1,input_string);
    strcpy(input_string_func_copy2,input_string);

    char *token;
    int argc = 0;
    char **argv;
    pid_t pid;
    int status;

    // Count the number of tokens considering backslash-escaped spaces
    token = strtok(input_string_func_copy1, " ");
    while (token != NULL) {
        while (token[strlen(token) - 1] == '\\') {
            token[strlen(token) - 1] = '\0'; // Remove the trailing backslash
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
        perror("malloc");
        return;
    }

    // Copy tokens into argv
    token = strtok(input_string_func_copy2, " ");
    for (int i = 0; i < argc; i++) {
        // Handle escaped spaces during copying
        while (token[strlen(token) - 1] == '\\') {
            token[strlen(token) - 1] = '\0'; // Remove the trailing backslash
            strcat(token, " ");
            char *next_part = strtok(NULL, " ");
            if (next_part != NULL) {
                strcat(token, next_part);
            } else {
                break;
            }
        }

        argv[i] = malloc(strlen(token) + 1);
        if (argv[i] == NULL) {
            perror("malloc");
            // Free previously allocated memory before exiting
            for (int j = 0; j < i; j++) {
                free(argv[j]);
            }
            free(argv);
            return;
        }
        strcpy(argv[i], token);
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL; // Null-terminate the argv array

    if(argv[0][0] != '/' )
    {
        // Fork a new process
        pid = fork();
        if (pid == -1) {
            perror("fork");
            // Free memory before exiting
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
            return;
        } else if (pid == 0) {
            // Child process
            if (execvp(argv[0], argv) == -1) {
                perror("execvp");
                // Free memory if exec fails
                for (int i = 0; i < argc; i++) {
                    free(argv[i]);
                }
                free(argv);
                exit(1);
            }
        } else {
            // Parent process
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                return;
            }

            // Process exit status
            if (WIFEXITED(status)) {
                add_process(pid,WEXITSTATUS(status));
            }

            // Free memory
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
        }
    }
    else 
    {
        // Fork a new process
        pid = fork();
        if (pid == -1) {
            perror("fork");
            // Free memory before exiting
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
            return;
        } else if (pid == 0) {
            // Child process
            if (execv(argv[0], argv) == -1) {
                perror("execv");
                // Free memory if exec fails
                for (int i = 0; i < argc; i++) {
                    free(argv[i]);
                }
                free(argv);
                exit(1);
            }
        } else {
            // Parent process
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                return;
            }

            // Process exit status
            if (WIFEXITED(status)) {
                add_process(pid,WEXITSTATUS(status));
            }

            // Free memory
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
        }        


    }
}

void fun_SHELL_WRITE()
{
    const char *shellMsg= "Yaa Naaa3m: ";
    const char *shellMsg2= " >>> ";
    int write_cheaker=0;

    fun_PWD();

    write_cheaker=write (STDOUT, shellMsg ,strlen(shellMsg));
    if(write_cheaker <= 0)
        {
            printf("Can't Read from user Shell will stop \n");
            perror("write");
            exit(1);
        }

     write_cheaker=write (STDOUT, pwd_PATH ,strlen(pwd_PATH));
    if(write_cheaker <= 0)
        {
            printf("Can't Read from user Shell will stop \n");
            perror("write");
            exit(1);
        }

    write_cheaker=write (STDOUT, shellMsg2 ,strlen(shellMsg2));
    if(write_cheaker <= 0)
        {
            printf("Can't Read from user Shell will stop \n");
            perror("write");
            exit(1);
        }
}

/*Main Function */
int main(void)
{
    char  *token; /*token that seperate the comand */
    char input_string_main_copy[INPUT_BUFFER]; /*to be used in strtok as strtok changes the string so we need the input to be not changed in extrnal command function*/
    int read_cheaker=0;

    while(1)
    {
        fun_SHELL_WRITE();

        read_cheaker=read (STDIN, input_string ,INPUT_BUFFER);
        if(read_cheaker <= 0)
        {
            printf("Can't Read from user Shell will stop \n");
            perror("read");
            break;
        }

        REMOVE_LeadingSpaces(input_string);

        if(input_string[0] != '\n' ){
            remove_newline(input_string);
        }

        strcpy(input_string_main_copy,input_string); 

        /*get the first token command */
        token = strtok(input_string_main_copy, " ");

        if(*token == '\r' || *token == '\n' ) //to use enter as in terminal
        {
            continue; 
        }
        else if(is_internal_command(token))
        {

            if(strcmp(token,"help") == 0 )
            {
                token=strtok(NULL, " ");
                if( token != NULL )
                {
                    fflush(stdout);
                    printf("Please Enter a valid command -_- use help to know all commands \n");
                    continue;
                }
                fun_HELP();
                continue;
            }
            else if(strcmp(token, "mypwd") == 0 )
            {
                token=strtok(NULL, " ");
                if( token != NULL )
                {
                    fflush(stdout);
                    printf("Please Enter a valid command -_- use help to know all commands \n");
                    continue;
                }
                fun_PWD();
                printf("%s \n",pwd_PATH);
                continue;
            }
            else if(strcmp(token, "myecho") == 0 )
            {
                fun_ECHO();
                continue;
            }
            else if (strcmp(token, "exit") == 0 )
            {
                token=strtok(NULL, " ");
                if( token != NULL )
                {
                    fflush(stdout);
                    printf("Please Enter a valid command -_- use help to know all commands \n");
                    continue;
                }
                printf("Hatw7shni y sa7by <_> \n5od balak 3la nfsk bye \n");
                break;  
            }
            else if(strcmp(token, "mycp") == 0 )
            {
                fun_COPY();
                continue;
            }
            else if (strcmp(token, "mymv") == 0 )
            {
                fun_MOVE();
                continue;
            }
            else if (strcmp(token, "cd") == 0)
            {
                fun_CD();
                continue;
            }
            else if(strcmp(token, "type") == 0)
            {
                fun_TYPE();
                continue;
            }
            else if(strcmp(token, "envir") == 0)
            {
                token=strtok(NULL, " ");
                if( token != NULL )
                {
                    if(strcmp(token, "variable") == 0)
                    {
                        token=strtok(NULL, " ");
                        if( token != NULL )
                        {
                            fflush(stdout);
                            printf("Please Enter a valid command -_- use help to know all commands \n");
                            continue;
                        }
                        fun_ENVIR_VARIABLE();
                        continue;

                    }
                    fflush(stdout);
                    printf("Please Enter a valid command -_- use help to know all commands \n");
                    continue;
                }
                fun_ENVIR();
                continue;
            }
            else if(strcmp(token, "phist") == 0 )
            {
                token=strtok(NULL, " ");
                if( token != NULL )
                {
                    fflush(stdout);
                    printf("Please Enter a valid command -_- use help to know all commands \n");
                    continue;
                }
                fun_PHIST();
                continue;
            }   
        }
        else if(is_external_command(token))
        {
            fun_EXTERNAL_COMMAND();
            continue;
        }
        else{
            fflush(stdout);
            printf("Please Enter a valid command -_- use help to know all commands \n");
        }

    }

    return 0;

}    
