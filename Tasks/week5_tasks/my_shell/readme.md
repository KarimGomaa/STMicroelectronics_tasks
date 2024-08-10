# 💻 Shell Implementation

Welcome to My Custom Shell Implementation ! This README.md provides an overview of the project, features, usage, and implementation details..

## 📚 Table of Contents
- [🌟 Project Overview](#-project-overview)
- [📚 Libraries Used](#-libraries-used)
- [📖 Manuals](#-manuals)
- [🎯 Features](#-features)
- [🚀 Installation & Usage](#-installation--usage)
- [🛠️ Internal Commands](#-internal-commands)
  - [🧭 Navigation Commands](#-navigation-commands)
  - [📤 Output Commands](#-output-commands)
  - [🖥️ Shell Control Commands](#-shell-control-commands)
  - [🗂️ File Management Commands](#-file-management-commands)
  - [🔧 Variables Commands](#-variables-commands)
  - [🖱️ System Information Commands](#-system-information-commands)
- [🔧 External Commands](#-external-commands)
  - [Functions in `external_commands.c`](#functions-in-external_commandsc)
- [🔍 Advice for Using `my_shell`](#-advice-for-using-my_shell)
    - [🔄 Redirection and Pipelines](#redirection-and-pipelines)
    - [⚙️Internal vs. External Commands](#internal-vs-external-commands)
- [📝 Main Function Overview](#main-function-overview)
- [🌐 Global Variables and Structures](#global-variables-and-structures)
- [🚫 Unsupported Commands](#-unsupported-commands)
- [🌟 Shell Preview](#-shell-preview)
- [🔚 Conclusion](#-conclusion)


## 🌟 Project Overview

This project involves developing a custom shell in C, spread across six files: `my_shell.c`, `my_shell.h`, `external_commands.c`, `external_commands.h`, `internal_commands.c`, and `internal_commands.h`. The shell handles internal, external, and unsupported commands, offering a basic yet functional command-line interface for user interaction.

We will go through each file, explaining its purpose and the key components it contains.


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
- `man 2 dup` : Used to redirect the process input,output and error.

## 🎯 Features

- **Internal Commands**: Built-in commands implemented directly within the shell. Use the `help` command in the shell to list and understand these commands.
- **External Commands**: Commands found in the environment variable locations.
- **Redirection Handling**: The shell can handle input and output redirection efficiently.
- **Pipelining**: Supports pipelining, allowing for the chaining of commands through an infinite number of processes.
- **Whitespace Handling**: The shell can manage spaces within commands using the backslash (`\`) to escape them in both internal and external commands.
- **Stability and Corner Case Support**: The shell has been built to handle a wide range of corner cases, ensuring high stability and robust performance.



## 🚀 Installation & Usage

1. **Clone the Repository**
   ```bash
   git clone <repository-url>

1. **Compile the Shell using Makefile**
   ```bash
   make
1. **Run the Shell**
   ```bash   
   ./my_shell

## 🛠️ Internal Commands

These commands are implemented directly within the shell in the `internal_commands.c` file and include:

#### 🧭 Navigation Commands

- **`cd`: Change Directory**
  - **Function:** `void fun_CD()`
  - **Description:** Changes the current working directory of the shell process using the `chdir` system call. If no directory is specified, the function defaults to the user's home directory.

- **`mypwd`: Print Working Directory**
  - **Function:** `void fun_PWD()`
  - **Description:** Retrieves and displays the current working directory using the `getcwd` function, storing the path in a buffer and outputting it to the terminal.

#### 📤 Output Commands

- **`myecho`: Echo Arguments**
  - **Function:** `void fun_ECHO()`
  - **Description:** Prints the given arguments to the terminal. It handles special characters and escape sequences, ensuring the output is formatted correctly.

- **`help`: Help Information**
  - **Function:** `void fun_HELP()`
  - **Description:** Displays a list of all internal commands with brief descriptions. Provides guidance on how to use the shell's built-in commands.

#### 🖥️ Shell Control Commands

- **`exit`: Exit Shell**
  - **Function:** `int is_internal_command(const char *command)`
  - **Description:** Although `exit` is handled as part of the `is_internal_command` function, it effectively terminates the shell when detected.

- **`type`: Command Type Information**
  - **Function:** `void fun_TYPE()`
  - **Description:** Determines if a command is internal, external, or unsupported by using checks like `is_internal_command` and `is_external_command`.

#### 🗂️ File Management Commands

- **`mycp`: Copy Files**
  - **Function:** `void fun_COPY()`
  - **Description:** Handles file copying by reading the source file and writing its contents to the destination file. Ensures valid paths and handles errors gracefully.

- **`mymv`: Move or Rename Files**
  - **Function:** `void fun_MOVE()`
  - **Description:** Moves or renames files using the `rename` system call. If moving across different file systems, it copies the file to the new location and deletes the original.

#### 🔧 Variables Commands

- **`envir`: Display Environment Variables**
  - **Function:** `void fun_ENVIR(char *token)`
  - **Description:** Iterates through and displays all environment variables. It can also display a specific environment variable if specified by the user.

- **`envir variable`: Set or Display Specific Environment Variable**
  - **Function:** `void fun_PLACE_VARIABLE(char *input_string_main_copy, char *equal_sign, char VARIABLE_MODE)`
  - **Description:** Sets or displays a specific environment variable. Parses the input to separate the variable name and value, updating or displaying it based on the mode.

- **`allvars`: Display All Variables**
  - **Function:** `void fun_ALLVARS(char *which_variables)`
  - **Description:** Displays all shell local and environment variables currently set     in the shell session it supports command like `allvars -l` to print local variables only.
  
#### 🖱️ System Information Commands

- **`phist`: Command History**
  - **Note:** While `phist` is recognized as an internal command, its implementation is in `external_commands.c`. It displays the history of the last 10 commands executed in the shell.

- **`free`: Display Memory Usage**
  - **Function:** `void fun_FREE()`
  - **Description:** Displays memory usage information, similar to the `free` command in Unix-like systems. It reads from system files or invokes system calls to show memory status.

- **`uptime`: Show System Uptime**
  - **Function:** `void fun_UPTIME()`
  - **Description:** Retrieves and displays the system's uptime and load averages, typically by reading from the `/proc/uptime` file on Linux systems.

## 🔧 External Commands

The shell can execute any external command available in the system's environment PATH using its name and it supports calling by its PATH . Some common examples include:

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

### Functions in `external_commands.c`

Here is a detailed explanation of each function in the `external_commands.c` file:
  
#### `add_process`

- **Purpose:** This function adds a new process to the `process_history` array, which keeps track of the processes executed by the shell.
- **Parameters:**
  - `pid`: Type `int` - The Process ID of the executed command.
  - `exit_status`: Type `int` - The exit status of the process.
  - `process_name`: Type `char*` - The name of the process executed.
- **Operation:**
  - If the `process_history` array is not full, the function stores the `pid`, `exit_status`, and `process_name` in the array and increments the `process_count`.
  - If the array is full, the function shifts the history to make room for the new entry by moving existing entries one position forward, overwriting the oldest entry.
  - This ensures that the `process_history` array always contains the most recent processes up to its maximum capacity.

#### `fun_PHIST`

- **Purpose:** This function likely implements the `PHIST` command, which is an internal command that displays the process history.
- **Parameters:** None
- **Operation:**
  - This function iterates over the `process_history` array and prints the stored information about each process, including the process ID, exit status, and name.

#### `is_external_command`

- **Purpose:** This function checks if a given command is an external command that can be executed by the shell.
- **Parameters:**
  - `command_name`: Type `const char*` - The name of the command to check.
- **Operation:**
  - The function searches through the system's `PATH` environment variable to determine if the command exists and can be executed. It returns a boolean value indicating whether the command is external.

#### `parse_redirection`

- **Purpose:** This function handles input,output and error redirection for external commands.
- **Parameters:**
    - `char *str`: it is the input_string provided by fun_EXTERNAL_COMMAND to be edited .
    - `char *redirect_pos`: it is the parameter to store the redirected symbol position wether it is ( < , > , 2>).
    - `char **redirected_file`: it is used to save the file location in input, output,error files in fun_EXTERNAL_COMMAND.
  - `is_error`: Type `char` - A flag indicating whether the redirection is for standard output (`stdout`) or standard error (`stderr`).
- **Operation:**
  - The function parses the command for redirection symbols (`>`, `<`, `2>`) and opens the corresponding files for output or error redirection. It then sets up the file descriptors accordingly.

#### `free_argv`

- **Purpose:** This function frees the memory allocated for the `argv` array, which stores command-line arguments.
- **Parameters:** `char ***argv` : sent by fun_PIPELINING.
- **Operation:**
  - The function is called by fun_PIPELINING and it iterates over the `argv` array and frees each argument string before freeing the `argv` array itself.

#### `parse_pipeling_first_step`

- **Purpose:** This function handles the first step in setting up a pipeline between commands.
- **Parameters:**
    - `char *str`: it is the input_string provided by fun_PIPELING to be edited .
    - `char *redirect_pos`: it is the parameter to store the pipelining symbol | position .
    - `char **redirected_file`: it is used to save the command after prasing it in command_buff provided by fun_PIPELING.
- **Operation:**
  - The function creates a pipe and sets up the necessary file descriptors for the first command in the pipeline. This ensures that the output of the first command is directed to the input of the next command in the pipeline.

#### `parse_pipeling_second_step`

- **Purpose:** This function handles the second step in setting up a pipeline between commands.
- **Parameters:**
    - `char ***argv`: used to save each command in its own array of pointers to characters to be sent to execv or execvp.
    - `in *argc`: the argc of the argv and it is array of size num_of_commands.
    - `k`: Type `int` - The index of the current command in the pipeline sequence.
  - `char *command_buff`: the command which we excuted from first stage and now we are going to put it in argv as execv and execvp needs argv and argc.
- **Operation:**
  - The function sets up the file descriptors for the current command in the pipeline, ensuring that it reads from the previous command's output and writes to the next command's input.

#### `fun_PIPELINING`

- **Purpose:** This function executes a series of commands connected by pipelines.
- **Parameters:** `char *pipelining_delimeter` : to know the postion of | in the string.
- **Operation:**
  - The function forks multiple child processes, each representing a command in the pipeline. It sets up the appropriate file descriptors for each process and then waits for all processes to complete.

### `fun_EXTERNAL_COMMAND`

- **Purpose:** This function executes an external command by forking a child process.
- **Parameters:** None
- **Operation:**
  - The function forks a child process and uses `execvp` to replace the child's process image with the external command. It waits for the command to complete and captures its exit status.


## 🔍 Advice for Using `my_shell`

- When using `cp` or `mv` with spaces in paths, these commands may fail. It is recommended to use `mycp` and `mymv` instead, as they now support paths with spaces.
- Other commands, such as `touch`, `rm`, etc., work well with the handling of spaces.

#### `mycp`
- To copy a file using the `mycp` command, ensure that the destination path does **not** end with a trailing slash `/`.
- Example:
  - **Correct:** `mycp /usr/karim/Desktop/karim.c /usr/karim`
  - **Incorrect:** `mycp /usr/karim/Desktop/karim.c /usr/karim/`

  In the correct usage, the file `karim.c` will be copied to the directory `/usr/karim/` and will retain its original name.

#### `mymv`
- To move or rename a file using the `mymv` command, include a trailing slash `/` at the end of the destination directory path.
- Example:
  - **Correct:** `mymv /usr/karim/Desktop/karim.c /usr/karim/`
  - **Incorrect:** `mymv /usr/karim/Desktop/karim.c /usr/karim`

  In the correct usage, the file `karim.c` will be moved to the directory `/usr/karim/`.

**Note:** The distinction between these commands helps ensure proper file placement and organization.

###🔄 Redirection and Pipelines

- You can now use redirection without worrying about the order in which it appears in the command line.
- Multiple pipelines are supported, allowing you to chain commands together.
- However, please note that pipeline redirection is not supported in this shell.

###⚙️ Internal vs. External Commands

- The shell handles all corner cases within its internal commands, providing a robust and reliable experience.
- However, you may encounter issues with certain external commands, such as `echo $variable` or `echo "karim"`, especially when they involve intercommand integration. For these cases, it is recommended to use the internal command instead.
- **Note:** External commands now handle spaces effectively, just like all other functions in the shell.

##📝 Main Function Overview

The `main` function is the entry point of the custom shell program. It contains the main loop that continuously prompts the user for input, processes commands, and executes the appropriate functions.

### How the Main Function Works

1. **Initialization and Loop Start**:
   - The shell initializes variables such as `token` for command separation and `input_string_main_copy` for safely copying the input string.
   - It enters an infinite loop (`while(1)`) to continuously prompt the user for commands.

2. **Prompt and Read Input**:
   - The function `fun_SHELL_WRITE()` is called to display the shell prompt. This prompt includes the current working directory and uses colored text for better readability.
   - The program reads user input from the standard input using `read(STDIN, input_string, INPUT_BUFFER)` and checks if the reading was successful.

3. **Input Sanitization**:
   - Leading spaces are removed from the input string with `REMOVE_LeadingSpaces(input_string)`, and newlines are removed with `remove_newline(input_string)`.

4. **Command Tokenization**:
   - The input string is copied to `input_string_main_copy` to prevent changes during tokenization.
   - The first token (command) is extracted using `strtok(input_string_main_copy, " ")`.

5. **Command Handling**:
   - **Internal Commands**: The program checks if the command is internal using `is_internal_command(token)` and executes the corresponding function (e.g., `fun_HELP()` for `help`).
   - **External Commands**: If the command is not internal, it checks if it is an external command using `is_external_command(token)` and executes it with `fun_EXTERNAL_COMMAND()`.
   - **Invalid Commands**: If the command is neither internal nor external, an error message is displayed, prompting the user to enter a valid command.

6. **Exit Condition**:
   - The loop continues indefinitely until the user inputs the `exit` command, which breaks the loop and terminates the shell session.


##🌐 Global Variables and Structures

The shell program uses several global variables and structures to manage state and functionality. Here’s an explanation of each:

- `char input_string[INPUT_BUFFER];`
  - **Description:** This global buffer holds the command input from the user and is used across all functions for accessing and manipulating the command entered.

- `char pwd_PATH[PWD_BUFFER];`
  - **Description:** This array stores the current working directory path and is used in commands like `mypwd` to display the directory.

- `extern char** environ;`
  - **Description:** This variable provides access to the environment variables for the current process, used in the `envir` function to display or manipulate environment variables.

- `static const char *internal_commands[] = {"cd", "mypwd", "myecho", "help", "exit", "type", "mycp", "mymv", "envir", "phist", "free", "uptime", "allvars", NULL};`
  - **Description:** This array lists all the internal commands supported by the shell, including those implemented in `external_commands.c`. It is used to check if a command is internal and to execute the corresponding function.

- `static int exit_from_fun_PATH_CHECKSPACES = 1;`
  - **Description:** A flag used to exit from the `fun_PATH_CHECKSPACES` while loop, controlling the flow during path checking operations.

- `static int return_variable = 0;`
  - **Description:** A global variable used in `fun_COPY` and `fun_MOVE` to return from the `fun_PATH_CONCATENATE` function if the filename ends with a backslash (`\`).

- `static local_variable local_vars[MAX_LOCAL_VARIABLES];`
  - **Description:** An array used to store local variables within the shell, allowing for temporary variable storage and manipulation during shell operation.

- `static int local_var_count = 0;`
  - **Description:** A counter that tracks the number of local variables currently in use, ensuring that the shell does not exceed its defined limit.

- `typedef struct { pid_t pid; int exit_status; } ProcessInfo;`
  - **Description:** This structure is used to store information about processes, including:
    - `pid`: The process ID.
    - `exit_status`: The exit status of the process.

- `static ProcessInfo process_history[MAX_HISTORY];`
  - **Description:** This global array stores the history of processes that have been executed, keeping track of process IDs and their exit statuses up to a maximum number defined by `MAX_HISTORY`.

- `static int process_count = 0;`
  - **Description:** A counter for the number of processes that have been added to `process_history`, starting from 0 and incrementing as processes are tracked.

- `static char redirection_error_occur = 0;`
  - **Description:** A flag used to indicate if a redirection error occurred during the execution of a command, helping the shell to handle such cases appropriately.

- `static char pipeline_exit_now = 0;`
  - **Description:** A flag used to indicate if an immediate exit is needed in the middle of a pipeline, ensuring proper pipeline execution control.

- `static int num_of_commands;`
  - **Description:** This variable stores the number of commands in the current input string, helping the shell to manage and execute multiple commands in sequence or pipeline.

These global variables and structures are essential for the shell's operation, allowing it to handle user input, track the current directory, manage environment variables, execute internal commands, and maintain a history of processes. They also provide control over various operations, such as redirection and pipeline management, ensuring the shell functions smoothly and reliably.

## 🚫 Unsupported Commands

The shell currently does not support any applications beyond the internal and external commands listed above. Specifically:

- **Options for `mycp` and `mymv`**: The custom `mycp` and `mymv` commands do not support options. They should be used simply as `mycp sourcepath destinationpath` and `mymv sourcepath destinationpath`.

These limitations are acknowledged, and we plan to address them in future implementations to provide more comprehensive support.

## 🌟 Shell Preview
![1](https://github.com/user-attachments/assets/b956c95b-00ec-433b-a83c-9a97867399d7)
![2](https://github.com/user-attachments/assets/ab5d1111-4d55-4954-9a7c-fef54d661b47)
![3](https://github.com/user-attachments/assets/db2eb14e-58db-44bd-9905-365626fff399)
![4](https://github.com/user-attachments/assets/568b27f2-61cb-4054-b05b-8826ff74ce1a)
![5](https://github.com/user-attachments/assets/68344350-1beb-4ec9-90c0-68a61689fcb4)

## 🔚 Conclusion

This custom shell program demonstrates the implementation of a simple command-line interpreter with a variety of internal and external commands. The project serves as a practical exploration of system programming concepts, including process management, command parsing, handling of environment variables, and supporting local variables.

The current version has made significant improvements, including support for file paths with spaces, handling external commands using full file paths, and enhancing error handling. Additionally, the program now supports custom commands like `mycp` and `mymv` with spaces in paths, as well as advanced features like command redirection and pipelining.

Future improvements could include supporting more complex command syntax, further enhancing error handling, solving any detected bugs, and adding more internal commands. These enhancements aim to provide a more robust and user-friendly shell experience.

We hope this project serves as a valuable learning experience and a foundation for further exploration in the field of system programming.


---

Thank you for using our my_shell program! If you have any questions or suggestions, please feel free to contribute or contact us.

Happy Coding!

