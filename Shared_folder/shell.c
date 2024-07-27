#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STDIN   (0)
#define STDOUT  (1)
#define STDERR  (2)

char input_string[100]; /*global value to get the command and use in all functions*/
char  *token; /*token that seperate the comand */


/* Function to print the working directory path */
void fun_PWD() 
{
	fflush(stdout);
	char pwd_PATH[100];
	getcwd(pwd_PATH,100);
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

void fun_ECHO()
{
	fflush(stdout);
	token = strtok(NULL, " ");
	while(token != NULL) 
	{
		printf("%s ",token);
		token = strtok(NULL, " ");
	}
}

void fun_COPY()
{
	token = strtok(NULL, " ");
	
		
		int fd_src = open(token,O_RDONLY);  // see man 2 open
		if (fd_src == -1)
		{
			fflush(stdout);
			printf( "Error in openning src file, make sure you entered the full path correct ^_^ \n");
			return ;
		}
		
		token = strtok(NULL, " ");
		int fd_test = open(token,O_RDONLY);
		if (fd_test != -1)
		{
			fflush(stdout);
			printf( "Error in destenation file, file already exist try change the name \n");
			return ;
		}

		int fd_dest = open(token,O_WRONLY | O_CREAT | O_TRUNC , 0644); 
		if (fd_dest == -1)
		{
			fflush(stdout);
			printf( "Error in destenation file, make sure you entered the full path correct ^_^ \n");
			return ;
		}

		char buff[100];
		int size=0;

		do 
		{
			size = read(fd_src,buff, 100);  // see man read
			write(fd_dest,buff,size);

		}while(size == 100);

		close(fd_src);
		close(fd_dest);
	
	fflush(stdout);
	printf("DONE ^-^ \n");
}

void fun_MOVE()
{


}

int main(void)
{

	ssize_t readSize=0;
	const char *shellMsg= "Ya Na3m >> ";

	while(1)
	{
		write (STDOUT, shellMsg ,strlen(shellMsg));

		readSize = read (STDIN, input_string , 100);

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
		else if(strcmp(token, "cp") == 0 || strcmp(token, "cp") == 10 )
		{
			fun_COPY();
			continue;
		}
		else if (strcmp(token, "mv") == 0 || strcmp(token, "mv") == 10 )
		{
			fun_MOVE();
			continue;
		}	

	}

    return 0;

}    
