# Shell Program

This is a simple shell program implemented in C that supports six basic functions. The program allows users to perform common shell tasks such as printing the working directory, echoing text, copying and moving files, getting help on supported commands, and terminating the program.

## Supported Commands

1. `pwd`: Prints the current working directory.
2. `echo`: Prints the provided text to the screen.
3. `cp`: Copies files from one path to another.
4. `mv`: Moves files from one path to another.
5. `help`: Lists all supported commands and their descriptions.
6. `exit`: Terminates the program.

## Implementation Details

The program makes use of various system calls and functions to implement the commands. Here are the relevant manual commands used:

- `man 2 open`: Used for opening files.
- `man getcwd`: Used to get the current working directory.
- `man read`: Used for reading from files.
- `man 2 write`: Used for writing to files.
- `man 2 stat`: Used for obtaining information about files.
- `man remove`: Used for removing files.

## Compilation

To compile the program, use the following command:

```sh
gcc -static -o exeshell shell.c

Program Explanation
The shell program operates in a loop, waiting for user input. Based on the input command, it performs one of the supported actions. Here's a brief overview of how each command works:

pwd: Uses getcwd to get the current working directory and prints it.
echo: Takes the input text and prints it to the screen.
cp: Copies a file from the source path to the destination path using open, read, and write.
mv: Moves a file from the source path to the destination path. It uses cp to copy the file and then remove to delete the original file.
help: Lists all the supported commands and their descriptions.
exit: Exits the program loop, terminating the shell.
This shell program is a basic implementation, meant for educational purposes to demonstrate how common shell functions can be implemented in C.

