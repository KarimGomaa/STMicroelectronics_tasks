# 🗂️ My ls
`myls` is a custom implementation of the `ls` command in C, designed to list the contents of a directory with various formatting options. It supports multiple flags for displaying files and directories in different formats, allowing users to customize their output according to their needs.

## 📚 Table of Contents 
- [🌟 Features](#features)
- [⚙️ Implementation Details](#implementation-details)
- [🚀 Usage](#usage)
- [📋 Functions](#functions)
- [🔄 Code Flow](#code-flow)
- [📹 My ls Illustration and Testing Video](#-my-ls-illustration-and-testing-video)
- [🎉 Conclusion](#conclusion)

## 🌟 Features
- **Flags**:
  - `-a`: Show all files, including hidden ones.
  - `-1`: Display one entry per line.
  - `-l`: Long listing format, showing detailed information.
  - `-f`: Disable sorting of files.
  - `-t`: Sort files by modification time.
  - `-u`: Sort files by last access time.
  - `-c`: Sort files by last status change time.
  - `-i`: Display inode numbers.
  - `-d`: List directories themselves, not their contents.

## ⚙️ Implementation Details
The program utilizes several key functions to manage the listing and formatting of files:

- **Error Handling**: The `PRINT_ERROR_FUN()` function provides user guidance for incorrect options.
- **Color Coding**: The `get_color_code()` function assigns ANSI color codes based on file types (e.g., directories, executables, symbolic links).
- **Permissions Display**: The `print_permissions()` function shows file permissions in a symbolic format, applicable to all file types.
- **Detailed Information**: The `print_detailed_info()` function mimics the `ls -l` command, displaying extensive details for each file, including symbolic links and their targets.
- **Handling Symbolic Links**: The program correctly identifies symbolic links and displays their target paths, ensuring users have full visibility into linked files.
- **Sorting**: The `cmpstringp()` function handles sorting based on user-defined flags, accommodating all types of files.
- **Output Formats**: The `HORIZONTAL_PRINTF_FUN()` and `VERTICAL_PRINTF_FUN()` functions format the output either horizontally or vertically, adapting to the type of files listed.


## 🚀 Usage
Compile the program using a C compiler with Makefile:

```bash
make
```

Run the program with the desired flags:
```bash
./myls [options] [directory]
```

## 📋 Functions

### 1. `PRINT_ERROR_FUN`
- Signature: `void PRINT_ERROR_FUN()`
- Description: Prints an error message when invalid options are provided. Guides the user on valid options.

### 2. `get_color_code`
- Signature: `static const char* get_color_code(COLOR color)`
- Description: Returns the ANSI color code corresponding to the specified file type.

### 3. `print_permissions`
- Signature: `static void print_permissions(mode_t mode)`
- Description: Prints file permissions in a symbolic format (e.g., rwxr-xr--) based on the file's mode.

### 4. `print_detailed_info`
- Signature: `static void print_detailed_info(const char *path, const char *filename, int i)`
- Description: Prints detailed information for a specified file, including permissions, owner, size, and last modified time.

### 5. `cmpstringp`
- Signature: `int cmpstringp(const void *p1, const void *p2)`
- Description: Comparison function used for sorting files. Compares based on modification, access, or status change times, or alphabetically if no flags are set.

### 6. `HORIZONTAL_PRINTF_FUN`
- Signature: `static void HORIZONTAL_PRINTF_FUN(int count, int maxlen)`
- Description: Prints file names in a horizontal grid format based on the terminal's width.
### 7. `VERTICAL_PRINTF_FUN`
- Signature: `static void VERTICAL_PRINTF_FUN(int count)`
- Description: Prints file names vertically, one per line.
### 8. `do_ls`
- Signature: `void do_ls(char *dir)`
- Description: Lists the contents of the specified directory. Applies the appropriate formatting based on the flags set. 


## 🔄 Code Flow

The flow of the `myls` program is structured as follows:

1. **Start**: The program begins execution from the `main` function.

2. **Option Parsing**:
   - The command-line arguments are processed using `getopt`.
   - Each option modifies global flags (e.g., `L_FLAG`, `ALL_FLAG`) to customize the behavior of the directory listing.

3. **Directory Argument Handling**:
   - After option parsing, the program checks for additional directory arguments.
   - If directories are specified, they are sorted using `qsort`.

4. **Directory Listing**:
   - The program iterates over the sorted directories.
   - For each directory, it prints the directory name and calls the `do_ls` function to list its contents.

5. **Default Behavior**:
   - If no directory arguments are provided, the program defaults to listing the current directory (`"."`).

6. **End**: The program concludes by returning an exit status code.

## 📹 My ls Illustration and Testing Video

This video provides an illustration of `myls` and demonstrates testing procedures. the video can be viewed via the following link:

[View Video](https://drive.google.com/file/d/1QDycgL9tBGbPgy0qUdRfzr5C8mhc7Vk4/view?usp=sharing)

## 🎉 Conclusion
The `myls` program offers a versatile alternative to the traditional `ls` command, providing users with a range of options for customizing their directory listings. Its implementation demonstrates an effective use of C programming principles and enhances the command-line experience for users seeking detailed and formatted directory information.

##
Thank you for exploring My ls project. If you have any questions or feedback, feel free to reach out or contribute to the repository.
