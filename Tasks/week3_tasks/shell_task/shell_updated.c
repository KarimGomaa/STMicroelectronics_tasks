/*******************************************************************************************************
* Name: shell.c
*
* Description: Mini Shell Program used to do the Following commands (pwd,echo,cp,mv,help,exit)
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

#define STDIN           (0)
#define STDOUT          (1)
#define STDERR          (2)
#define INPUT_BUFFER   (100)
#define READ_BUFFER    (1024)
#define PWD_BUFFER     (100)

/*Function to remove newlines from the token given by the user.*/
void remove_newline(char *str) {
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) {
        *pos = '\0';
    }
}

/* Function to print the working directory path */
void fun_PWD() 
{
	fflush(stdout);
	char pwd_PATH[PWD_BUFFER];
	getcwd(pwd_PATH,PWD_BUFFER);
	printf("%s \n",pwd_PATH);
}	

/*function to get help */
void fun_HELP()
{
	fflush(stdout);
	printf("Supported Commands: \n");
	printf(" 1) pwd: to print the working directory \n 2) echo: to print the text \n");
	printf(" 3) cp: to copy the files from path to another \n 4) mv: to move the files from path to another \n 5) help: to get all the commands suppurted \n");
	printf(" 6) exit: to terminate the program\n");
}

/*Function to print on the screen*/
void fun_ECHO()
{
	fflush(stdout);
	char* echo_variable = strtok(NULL, " ");
	while(echo_variable != NULL) 
	{
		printf("%s ",echo_variable);
		echo_variable = strtok(NULL, " ");
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

    remove_newline(sourcePath);

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

    remove_newline(targetPath);

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
    do 
		{
			size = read(fd_src,buff,READ_BUFFER);  // see man read
			write(fd_dest,buff,size);

		}while(size == 100);

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

    remove_newline(sourcePath);

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

    remove_newline(targetPath);

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

/*Main Function */
int main(void)
{

    char input_string[INPUT_BUFFER]; /* to get the command and use in all functions*/
	const char *shellMsg= "Ya Na3m >> ";
    char  *token; /*token that seperate the comand */

	while(1)
	{
		write (STDOUT, shellMsg ,strlen(shellMsg));

		read (STDIN, input_string ,INPUT_BUFFER);

        if(input_string[0] != '\n' ){
        remove_newline(input_string);
        }

		/*get the first token command */
		token = strtok(input_string, " ");

		if(strcmp(token,"help") == 0 || strcmp(token, "help") == 10 )
		{
			fun_HELP();
			continue;
		}
		else if(strcmp(token, "pwd") == 0 || strcmp(token, "pwd") == 10 )
		{
			fun_PWD();
			continue;
		}
			else if(strcmp(token, "echo") == 0 || strcmp(token, "echo") == 10 )
		{
			fun_ECHO();
			continue;
		}
		else if (strcmp(token, "exit") == 0 || strcmp(token, "exit") == 10 )
		{
			printf("Hatw7shni y sa7by <_> \n5od balak 3la nfsk bye \n");
			break;	
		}
		else if(strcmp(token, "cp") == 0 )
		{
			fun_COPY();
			continue;
		}
		else if (strcmp(token, "mv") == 0 )
		{
			fun_MOVE();
			continue;
		}	
		else if(*token == '\r' || *token == '\n' ) //to use enter as in terminal
		{
			continue; 
		}
		else{
			fflush(stdout);
			printf("Please Enter a valid command -_- use help to know all commands \n");
		}

	}

    return 0;

}    
