# Shell Implementation

Welcome to My Custom Shell Implementation Task Version 2! This README.md provides an overview of the project, features, usage, and implementation details. This version includes solved bugs and important advice for using the shell effectively.

## Table of Contents

1. [Project Overview](#-project-overview)
2. [Libraries Used](#-libraries-used)
3. [Features](#-features)
   - [Internal Commands](#-internal-commands)
   - [External Commands](#-external-commands)
4. [Installation & Usage](#-installation--usage)
5. [Internal Commands](#-internal-commands)
6. [External Commands](#-external-commands)
7. [Solved Bugs](#-solved-bugs)
   - [Spaces in `mycp`, `mymv`, `cd`, and External Commands](#-spaces-in-mycp-mymv-cd-and-external-commands)
    - [External Command Full Paths](#-external-command-full-paths)
   - [Bug in `echo`](#-bug-in-echo)
   - [System Calls Return Values](#-system-calls-return-values)

8. [Advice for Using `shell_v2`](#-advice-for-using-shell_v2)
9. [Unsupported Commands](#-unsupported-commands)
10. [Manuals](#-manuals)
11. [Implementation Details](#-implementation-details)
   - [Functions Used](#-functions-used)
   - [Main Function Overview](#-main-function-overview)
   - [Global Variables and Structures](#-global-variables-and-structures)
12. [Conclusion](#-conclusion)


## 🌟 Project Overview

This project involves developing a custom shell in C. The shell handles internal, external, and unsupported commands, offering a basic yet functional command-line interface for user interaction.

### 📚 Libraries Used

- `#include <stdio.h>`
- `#include <string.h>`
- `#include <stdlib.h>`
- `#include <unistd.h>`
- `#include <sys/types.h>`
- `#include <sys/stat.h>`
- `#include <fcntl.h>`
- `#include <sys/wait.h>`

To understand the usage and functions provided by these libraries, Just read the man pages that would be provided in the manuals section.

## 🎯 Features
- **Internal Commands**: Built-in commands implemented directly within the shell use help in shell to know these commands.
- **External Commands**: Commands found in the enivroment variable locations.
- **Unsupported Commands**: Commands that are not supported by the shell.


## 🚀 Installation & Usage

1. **Clone the Repository**
   ```bash
   git clone <repository-url>
1. **Compile the Shell**
   ```bash
   gcc -o shell shell_v2.c
1. **Run the Shell**
   ```bash   
   ./shell

## 🛠️ Internal Commands
These commands are implemented directly within the shell and include:

- `cd`: Change the current directory.
- `mypwd`: Display the current working directory.
- `myecho`: Print the given arguments to the terminal.
- `help`: Display help information about the shell's built-in commands.
- `exit`: Exit the shell.
- `type`: Indicate how a command would be interpreted (e.g., internal, external, or unsupported).
- `mycp`: Copy files from one location to another.
- `mymv`: Move or rename files.
- `envir`: Display all environment variables.
- `envir variable`: Set or display a specific environment variable.
- `phist`: Show the history of the last 10 commands executed in the shell.


## 🔧 External Commands

The shell can execute any external command available in the system's environment PATH Using its Name. Some common examples include:

- `ls`: List directory contents.
- `grep`: Search for patterns in files.
- `cat`: Concatenate and display file contents.
- `mkdir`: Create a new directory.
- `rm`: Remove files or directories.
- `cp`: Copy files or directories.
- `mv`: Move or rename files or directories.
- `echo`: Display a line of text or a string.
- `ps`: Report a snapshot of current processes.
- `top`: Display Linux tasks.

These external commands are executed by spawning child processes, allowing the shell to leverage the existing utilities provided by the operating system.

## 🐛 Solved Bugs

We have addressed and resolved the following issues:

- **Spaces in `mycp`, `mymv`, `cd`, and External Commands**: The shell now supports spaces in command paths by using a backslash (`\`) before each space. This fix applies to custom commands like `mycp` and `mymv`, as well as the built-in `cd` command and external commands.

- **External Command Full Paths**: The shell now supports executing external commands using their full file paths, in addition to command names found in the system's PATH environment variable. For example, the shell can now execute `/usr/bin/ls` as well as `ls`.

- **Bug in `echo`**: A bug causing unintended behavior in the `echo` command was identified and fixed.

- **System Calls return values**: We have enhanced error checking on system calls to ensure that everything works correctly.

## 🔍 Advice for Using `shell_v2`:
- When using `cp` or `mv` with spaces in paths, these commands may fail. It is recommended to use `mycp` and `mymv` instead, as they now support paths with spaces.
- Other commands, such as `touch`, `rm`, etc., work well with the handling of spaces.

#### `mycp`
- To copy a file using the `mycp` command, ensure that the destination path does **not** end with a trailing slash `/`.
- Example:
  - Correct: `mycp /usr/karim/Desktop/karim.c /usr/karim`
  - Incorrect: `mycp /usr/karim/Desktop/karim.c /usr/karim/`

  In the correct usage, the file `karim.c` will be copied to the directory `/usr/karim/` and will retain its original name.

#### `mymv`
- To move or rename a file using the `mymv` command, include a trailing slash `/` at the end of the destination directory path.
- Example:
  - Correct: `mymv /usr/karim/Desktop/karim.c /usr/karim/`
  - Incorrect: `mymv /usr/karim/Desktop/karim.c /usr/karim`

  In the correct usage, the file `karim.c` will be moved to the directory `/usr/karim/`.

**Note**: The distinction between these commands helps ensure proper file placement and organization.


## 🚫 Unsupported Commands

The shell currently does not support any applications beyond the internal and external commands listed above. Specifically:

- **Options for `mycp` and `mymv`**: The custom `mycp` and `mymv` commands do not support options. They should be used simply as `mycp sourcepath destinationpath` and `mymv sourcepath destinationpath`.

These limitations are acknowledged, and we plan to address them in future implementations to provide more comprehensive support.


## 📖 Manuals

The program makes use of various system calls and functions to implement the commands. Here are the relevant manual commands used:

- `man 2 open`: Used for opening files.
- `man getcwd`: Used to get the current working directory.
- `man read`: Used for reading from files.
- `man 2 write`: Used for writing to files.
- `man 2 stat`: Used for obtaining information about files.
- `man remove`: Used for removing files.
- `man envir`: Used for handling environment variables.
- `man chdir`: Used for changing the current working directory.
- `man getenv`: Used for retrieving environment variable values.
- `man access`: Used for checking user's permissions for a file.
- `man exec`: Used for executing programs.
- `man waitpid`: Used for waiting for state changes in a child process.
- `man exit`: Used for exiting a child process when execv is failed.

## 🏗️ Implementation Details

The implementation of the custom shell involves various functions, each handling specific tasks related to command execution and shell behavior. Below is a list of functions and global variable used in the implementation :

### Functions Used

- `add_process`: Manages the addition of processes to the shell's internal data structures for use in the `phist` command.
- `fun_PHIST`: Handles the display of the last 10 processes, showing the PID and exit status.
- `fun_PATH_CONCATENATE`: Concatenates paths for command execution.
- `fun_PATH_CHECKSPACES`: Checks and handles spaces in paths for external command execution.
- `is_internal_command`: Checks if a given command is an internal command, used in `main` and `type` commands.
- `is_external_command`: Checks if a given command is an external command, used in `main` and `type` commands.
- `fun_TYPE`: Determines the type of a command (internal, external, unsupported).
- `remove_newline`: Removes newline characters from a string, used after reading `input_string` in `main`.
- `REMOVE_LeadingSpaces`: Removes leading spaces from a string, used after `remove_newline` in `main`.
- `fun_PWD`: Implements the `mypwd` command to display the current directory.
- `fun_HELP`: Provides help information for shell commands.
- `fun_ECHO`: Implements the `myecho` command to print arguments.
- `fun_COPY`: Handles the `mycp` command for copying files.
- `fun_MOVE`: Implements the `mymv` command for moving/renaming files.
- `fun_CD`: Changes the current directory (`cd` command).
- `fun_ENVIR`: Displays environment variables (`envir` command).
- `fun_ENVIR_VARIABLE`: Sets or displays a specific environment variable.
- `fun_EXTERNAL_COMMAND`: Executes external commands found in the PATH.
- `fun_SHELL_WRITE`: Handles writing output to the shell.


### Main Function Overview

The `main` function is the entry point of the custom shell program. It contains the main loop that continuously prompts the user for input, processes commands, and executes the appropriate functions.

### How the Main Function Works

1. **Initialization and Loop Start**:
   - The shell starts by initializing necessary variables, including `token` for command separation and `input_string_main_copy` for safely copying the input string.
   - It enters an infinite loop (`while(1)`) to continuously prompt the user for commands.

2. **Prompt and Read Input**:
   - The function `fun_SHELL_WRITE()` is called to display the shell prompt.
   - It then reads user input from the standard input using `read(STDIN, input_string, INPUT_BUFFER)`.
   - Leading spaces are removed from the input string with `REMOVE_LeadingSpaces(input_string)`, and newlines are removed with `remove_newline(input_string)`.

3. **Command Tokenization**:
   - The input string is copied to `input_string_main_copy` to prevent modifications.
   - The first token (command) is extracted using `strtok(input_string_main_copy, " ")`.

4. **Command Handling**:
   - **Internal Commands**: The program checks if the command is internal using `is_internal_command(token)`.
     - It handles various internal commands such as `help`, `mypwd`, `myecho`, `exit`, `mycp`, `mymv`, `cd`, `type`, `envir`, `envir variable`, and `phist`. For each command, the corresponding function is called (e.g., `fun_HELP()` for `help`).
   - **External Commands**: If the command is not internal, it checks if it's an external command with `is_external_command(token)`. If so, `fun_EXTERNAL_COMMAND()` is called to execute it.
   - **Invalid Commands**: If the command is neither internal nor external, it prompts the user to enter a valid command.

5. **Exit Condition**:
   - The loop continues indefinitely until the user enters the `exit` command, which breaks the loop and terminates the shell.

### Global Variables and Structures:

The shell program uses several global variables and structures to manage state and functionality. Here’s an explanation of each:

- `char input_string[INPUT_BUFFER];`
  - This global variable stores the input command from the user. It is used across multiple functions to access and manipulate the command entered.

- `char pwd_PATH[PWD_BUFFER];`
  - This variable stores the current working directory path. It is used in commands like `mypwd` to display the directory.

- `extern char** environ;`
  - This variable provides access to the environment variables for the current process. It is used in the `envir` function to display or manipulate environment variables.

- `const char *internal_commands[] = {"cd", "mypwd", "myecho", "help", "exit", "type", "mycp", "mymv", "envir", "envir variable", "phist", NULL};`
  - This array lists all the internal commands supported by the shell. It is used to check if a command is internal and to execute the corresponding function.

- `typedef struct { pid_t pid; int exit_status; } ProcessInfo;`
  - This structure is used to store information about processes. It includes:
    - `pid`: The process ID.
    - `exit_status`: The exit status of the process.

- `ProcessInfo process_history[MAX_HISTORY];`
  - This global array stores the history of processes that have been executed. It keeps track of process IDs and their exit statuses up to a maximum number defined by `MAX_HISTORY`.

- `int process_count = 0;`
  - This integer keeps track of the number of processes that have been added to `process_history`. It starts from 0 and increments as processes are tracked.

These global variables and structures are essential for the shell's operation, allowing it to handle user input, track the current directory, manage environment variables, execute internal commands, and maintain a history of processes.

## 🔚 Conclusion

This custom shell program demonstrates the implementation of a simple command-line interpreter with a variety of internal and external commands. The project serves as a practical exploration of system programming concepts, including process management, command parsing, and handling of environment variables.

The current version has made significant improvements, including support for file paths with spaces, handling external commands using full file paths, and enhancing error handling. Additionally, the program now supports custom commands like `mycp` and `mymv` with spaces in paths.

Future improvements could include supporting more complex command syntax, further enhancing error handling, solving any detected bugs and adding more internal commands. These enhancements aim to provide a more robust and user-friendly shell experience.

We hope this project serves as a valuable learning experience and a foundation for further exploration in the field of system programming.


---

Thank you for using our shell_v2 program! If you have any questions or suggestions, please feel free to contribute or contact us.

Happy Coding!

