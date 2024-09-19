/******************************************************************************
 *
 * File Name: myls_functions.c
 *
 * Description: Source file for Functions and options in myls program
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include "myls_functions.h"

/* 
 * Global Flags
 * -------------
 * These flags are used to control the behavior of the directory listing program.
 */
char ALL_FLAG = 0; /* Flag to show all files (including hidden files) */
char ONE_FLAG = 0; /* Flag to display one entry per line */
char L_FLAG = 0; /* Flag for long listing format */
char F_FLAG = 0; /* Flag to disable sorting of files */
char T_FLAG = 0; /* Flag to sort files by modification time */
char U_FLAG = 0; /* Flag to sort files by access time */
char C_FLAG = 0; /* Flag to sort files by last status change time */
char I_FLAG = 0; /* Flag to display inode numbers */
char D_FLAG = 0; /* Flag to display the directory itself */

/* 
 * Structure to hold information about files 
 * -------------------------------------------
 * This structure stores the name, inode, and color information for each file.
 */
static struct file_info *file_list = NULL; /* Pointer to an array of file_info structures */

/* 
 * Global Variables
 * ----------------
 * These variables are used to store path and color information.
 */
static char symbolic_link_path[512]; /* Buffer to store the path of symbolic links used in print_detailed_info function*/
static char *global_dir; /* Pointer to hold the directory path for sorting used in cmpstringp function */

/* 
 * External Variables
 * ------------------
 * This variable is used to get the last error number set by system calls.
 */
extern int errno; /* External variable to access the last error number */



/* 
 * Function to print an error message for invalid options. 
 * This function doesn't take any parameters and prints an error message
 * guiding the user on how to use the correct command-line options.
 */
void PRINT_ERROR_FUN() 
{
    /* Print the error message and guide the user on the valid options. */
    printf("Sorry, you entered a wrong option. Please try again using only the following options:\n");
    printf("  -l  : List files in long format (detailed information including permissions, owner, size, and date).\n");
    printf("  -a  : Include hidden files (those starting with a dot '.').\n");
    printf("  -t  : Sort by modification time, newest first.\n");
    printf("  -u  : Sort by last access time.\n");
    printf("  -c  : Sort by last status change time.\n");
    printf("  -i  : Display inode numbers.\n");
    printf("  -f  : Disable sorting (show files in directory order).\n");
    printf("  -d  : List directories themselves, not their contents.\n");
    printf("  -1  : List one file per line.\n");
}


/* 
 * Function to get the color code for a file based on its type. 
 * It takes an enum COLOR type and returns the corresponding 
 * ANSI color code as a string.
 */
static const char* get_color_code(COLOR color) {
    switch (color) {
        /* Case for directories: Blue color. */
        case BLUE:
            return "\x1b[1;34m";  /* Blue for directories */
        
        /* Case for executable files: Green color. */
        case GREEN:
            return "\x1b[1;32m";  /* Green for executable files */
        
        /* Case for symbolic links: Cyan color. */
        case CYAN:
            return "\033[1;36m";  /* Cyan for symbolic links */
        
        /* Default case: Reset to default color. */
        default:
            return "\x1b[0m";   /* Reset to default color */
    }
}


/* 
 * Function to print the file permissions in a symbolic format (e.g., "rwxr-xr--"). 
 * It takes a mode_t value that represents the file's mode and prints 
 * the permissions along with the file type.
 */
static void print_permissions(mode_t mode)
{
    char str[11];  /* To store file type and permissions as a string */

    /* Determine the file type and store it in the first character of the string */
    str[0] = S_ISDIR(mode) ? 'd' :     /* Directory */
        S_ISLNK(mode) ? 'l' :          /* Symbolic link */
        S_ISCHR(mode) ? 'c' :          /* Character device */
        S_ISBLK(mode) ? 'b' :          /* Block device */
        S_ISFIFO(mode) ? 'p' :         /* FIFO (named pipe) */
        S_ISSOCK(mode) ? 's' :         /* Socket */
        '-';                           /* Regular file */

    /* Owner permissions */
    str[1] = (mode & S_IRUSR) ? 'r' : '-';  /* Read permission for owner */
    str[2] = (mode & S_IWUSR) ? 'w' : '-';  /* Write permission for owner */
    
    /* Execute permission for owner and setuid bit */
    if (mode & S_ISUID) {
        str[3] = (mode & S_IXUSR) ? 's' : 'S';  /* setuid bit is set */
    } else {
        str[3] = (mode & S_IXUSR) ? 'x' : '-';  /* Regular execute permission */
    }

    /* Group permissions */
    str[4] = (mode & S_IRGRP) ? 'r' : '-';  /* Read permission for group */
    str[5] = (mode & S_IWGRP) ? 'w' : '-';  /* Write permission for group */
    
    /* Execute permission for group and setgid bit */
    if (mode & S_ISGID) {
        str[6] = (mode & S_IXGRP) ? 's' : 'S';  /* setgid bit is set */
    } else {
        str[6] = (mode & S_IXGRP) ? 'x' : '-';  /* Regular execute permission */
    }

    /* Others permissions */
    str[7] = (mode & S_IROTH) ? 'r' : '-';  /* Read permission for others */
    str[8] = (mode & S_IWOTH) ? 'w' : '-';  /* Write permission for others */
    
    /* Execute permission for others and sticky bit */
    if (mode & S_ISVTX) {
        str[9] = (mode & S_IXOTH) ? 't' : 'T';  /* Sticky bit is set */
    } else {
        str[9] = (mode & S_IXOTH) ? 'x' : '-';  /* Regular execute permission */
    }

    /* Null-terminate the string to ensure proper string formatting */
    str[10] = '\0';  

    /* Print the formatted permissions string */
    printf("%10s ", str);
}


/* 
 * Function to print detailed file information, similar to the 'ls -l' command.
 * It takes the file's path, filename, and an index `i` for color code determination.
 */
static void print_detailed_info(const char *path, const char *filename, int i)
{
    struct stat info;  /* Struct to store file information */
    char fullpath[512]; /* Buffer to hold the full file path */

    /* Construct the full file path by combining the directory path and filename */
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filename);

    /* Get file status using lstat. If it fails, print an error and return. */
    if (lstat(fullpath, &info) == -1) {
        perror("lstat");
        return;
    }

    /* Print the file type and permissions */
    print_permissions(info.st_mode);

    /* Print the number of hard links (dynamic width is handled by %2ld) */
    printf("%2ld ", info.st_nlink);

    /* Get owner and group information using UID and GID */
    struct passwd *pw = getpwuid(info.st_uid);
    struct group *gr = getgrgid(info.st_gid);

    /* Calculate the lengths of the owner and group names for dynamic formatting */
    int user_len = strlen(pw->pw_name);
    int group_len = strlen(gr->gr_name);

    /* Print the owner and group names with the calculated lengths */
    printf("%*s %*s ", user_len, pw->pw_name, group_len, gr->gr_name);

    /* Print the file size */
    printf("%5ld ", info.st_size);

    /* Format and print the last modified time */
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&info.st_mtime));
    int time_len = strlen(timebuf);
    printf("%*s ", time_len, timebuf);

    /* Print the file name with color handling based on the index `i` */
    if (i >= 0) {
        printf("%s", get_color_code(file_list[i].CHOOSEN_COLOR));
    } else {
        printf("%s", get_color_code(BLUE)); /* Default to blue for directories */
    }

    printf("%s ", filename);  /* Print the file name */
    printf("%s", get_color_code(RESET)); /* Reset the color */

    /* Handle symbolic links (color cyan) and print the linked path */
    if (i >= 0 && get_color_code(file_list[i].CHOOSEN_COLOR) == "\033[1;36m") {
        printf("-> %s%s%s", get_color_code(BLUE), symbolic_link_path, get_color_code(RESET));
    }

    printf("\n"); /* Move to the next line after printing all details */
}


/* 
 * Comparison function for qsort to compare file paths.
 * It checks the sorting flags (T_FLAG, U_FLAG, C_FLAG) and compares 
 * file times accordingly. If no flags are set, it compares file names alphabetically.
 */
int cmpstringp(const void *p1, const void *p2)
{
    /* If any sorting flag (T_FLAG, U_FLAG, C_FLAG) is set, compare based on times */
    if (T_FLAG || U_FLAG || C_FLAG) {
        /* Cast to file_info structure */
        struct file_info *file1 = (struct file_info *)p1;
        struct file_info *file2 = (struct file_info *)p2;

        /* Retrieve file status information */
        struct stat stat1, stat2;
        char filepath1[512];
        char filepath2[512];

        /* Construct full paths for both files */
        snprintf(filepath1, sizeof(filepath1), "%s/%s", global_dir, file1->name);
        snprintf(filepath2, sizeof(filepath2), "%s/%s", global_dir, file2->name);

        /* Get file status for the first file */
        if (lstat(filepath1, &stat1) == -1) {
            return 0;
        }

        /* Get file status for the second file */
        if (lstat(filepath2, &stat2) == -1) {
            return 0;
        }

        /* Compare modification times (T_FLAG) */
        if (T_FLAG) {
            /* Compare seconds and nanoseconds of modification times */
            if (stat1.st_mtim.tv_sec < stat2.st_mtim.tv_sec)
                return 1;   /* file1 is older */
            else if (stat1.st_mtim.tv_sec > stat2.st_mtim.tv_sec)
                return -1;  /* file1 is newer */
            else {
                /* If seconds are equal, compare nanoseconds */
                if (stat1.st_mtim.tv_nsec < stat2.st_mtim.tv_nsec)
                    return 1; /* file1 is older */
                else if (stat1.st_mtim.tv_nsec > stat2.st_mtim.tv_nsec)
                    return -1; /* file1 is newer */
                else
                    return 0; /* Times are equal */
            }
        } 
        /* Compare access times (U_FLAG) */
        else if (U_FLAG) {
            if (stat1.st_atim.tv_sec < stat2.st_atim.tv_sec)
                return 1;   /* file1 was accessed earlier */
            else if (stat1.st_atim.tv_sec > stat2.st_atim.tv_sec)
                return -1;  /* file1 was accessed later */
            else {
                /* Compare nanoseconds if seconds are equal */
                if (stat1.st_atim.tv_nsec < stat2.st_atim.tv_nsec)
                    return 1; /* file1 was accessed earlier */
                else if (stat1.st_atim.tv_nsec > stat2.st_atim.tv_nsec)
                    return -1; /* file1 was accessed later */
                else
                    return 0; /* Access times are equal */
            }
        } 
        /* Compare status change times (C_FLAG) */
        else if (C_FLAG) {
            if (stat1.st_ctim.tv_sec < stat2.st_ctim.tv_sec)
                return 1;   /* file1 had its status changed earlier */
            else if (stat1.st_ctim.tv_sec > stat2.st_ctim.tv_sec)
                return -1;  /* file1 had its status changed later */
            else {
                /* Compare nanoseconds if seconds are equal */
                if (stat1.st_ctim.tv_nsec < stat2.st_ctim.tv_nsec)
                    return 1; /* file1's status changed earlier */
                else if (stat1.st_ctim.tv_nsec > stat2.st_ctim.tv_nsec)
                    return -1; /* file1's status changed later */
                else
                    return 0; /* Status change times are equal */
            }
        }
    } 
    else {
        /* If no flags are set, compare file names alphabetically */
        struct file_info *file1 = (struct file_info *)p1;
        struct file_info *file2 = (struct file_info *)p2;

        /* Use strcmp to compare file names */
        return strcmp(file1->name, file2->name);
    }
}


/* 
 * Function: HORIZONTAL_PRINTF_FUN 
 * -------------------------------
 * This function prints a list of files in a horizontal grid format, 
 * fitting as many file names as possible into each row based on the terminal's width.
 * 
 * Parameters:
 *   - int count: the total number of files to display.
 *   - int maxlen: the maximum length of the file names in the list.
 */
static void HORIZONTAL_PRINTF_FUN(int count, int maxlen)
{
    /* Get terminal width using ioctl and store it in a struct */
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); // Retrieves the terminal size
    int term_width = w.ws_col;  /* Terminal width in characters */

    /* Calculate the number of columns based on max filename length and terminal width */
    int col_width = maxlen + 2; /* Minimum width for each column (add padding for spacing) */
    int cols;  /* Variable to store the number of columns */

    /* If I_FLAG is not set, calculate number of columns normally */
    if (!I_FLAG)
        cols = term_width / col_width;
    else
        /* If I_FLAG is set, adjust the width to account for inode column and extra spacing */
        cols = (term_width - 12) / (col_width + 10);    

    /* Ensure there's at least one column even if the terminal width is small */
    if (cols == 0) {
        cols = 1;               
    }

    /* Loop through the list of files and print them in a grid format */
    for (int i = 0; i < count; i++) {
        /* If I_FLAG is set, print the inode of the file */
        if (I_FLAG) {
            printf("%-*lu ", 8, file_list[i].inode);  /* Print inode with padding */
        }

        /* Print the color associated with the file type */
        printf("%s", get_color_code(file_list[i].CHOOSEN_COLOR));

        /* Print the file name with column width padding */
        printf("%-*s", col_width, file_list[i].name);   

        /* Reset the color back to default after printing the file name */
        printf("%s", get_color_code(RESET));

        /* Print a newline when the row is filled or if it’s the last file */
        if ((i + 1) % cols == 0 || i == count - 1) {
            printf("\n");       /* Newline after filling a row or on the last item */
        }
    }
}


/* 
 * Function: VERTICAL_PRINTF_FUN 
 * -----------------------------
 * This function prints a list of files vertically, one per line.
 * 
 * Parameters:
 *   - int count: the total number of files to display.
 */
static void VERTICAL_PRINTF_FUN(int count)
{
    /* Loop through the list of files and print them one per line */
    for (int i = 0; i < count; i++) {
        /* If I_FLAG is set, print the inode of the file */
        if (I_FLAG) {
            printf("%-*lu ", 8, file_list[i].inode);  /* Print inode with padding */
        }

        /* Print the color associated with the file type */
        printf("%s", get_color_code(file_list[i].CHOOSEN_COLOR));

        /* Print the file name */
        printf("%s", file_list[i].name);

        /* Reset the color back to default and print a newline */
        printf("%s \n", get_color_code(RESET));
    }
}


/* 
 * Function: do_ls 
 * -----------------
 * This function lists the contents of a specified directory, printing 
 * details based on various flags that control the output format.
 * 
 * Parameters:
 *   - char *dir: the directory path to list.
 */
void do_ls(char *dir)
{
    /* Declare a pointer to hold directory entries */
    struct dirent *entry;

    /* If D_FLAG is set, just display the directory itself */
    if (D_FLAG) {
        /* If I_FLAG is set, print the inode of the directory */
        if (I_FLAG) {
            struct stat dir_stat; /* Structure to hold file status information */
            /* Get the status of the directory */
            if (lstat(dir, &dir_stat) == -1) {
                perror("lstat"); /* Print error message if lstat fails */
                return; /* Exit the function */
            }
            printf("%-*lu ", 8, dir_stat.st_ino);   /* Print inode if I_FLAG is set */
        }

        /* If L_FLAG is set, print detailed information about the directory */
        if (L_FLAG) {
            print_detailed_info(".", dir, -1);  /* Use current directory to get details of dir */
        } else {
            /* If F_FLAG is not set, print directory name in blue */
            if (!F_FLAG) {
                printf("%s", get_color_code(BLUE));
            }
            printf("%s\n", dir);    /* Print directory name */
            printf("%s", get_color_code(RESET)); /* Reset color */
        }
        return; /* Don't list contents of the directory */
    }

    /* Open the specified directory */
    DIR *dp = opendir(dir);
    /* Check if the directory was opened successfully */
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir); /* Print error message */
        return; /* Exit the function */
    }

    int count = 0; /* Counter for the number of files */
    int maxlen = 0; /* Variable to store the maximum length of file names */

    /* Variable to calculate the total block size for L_FLAG */
    struct stat file_stat; /* Structure to hold file status information */
    int total_blocks = 0; /* Variable to accumulate total block size */

    /* Read all directory entries and store their names */
    while ((entry = readdir(dp)) != NULL) {
        /* Skip hidden files (those starting with '.') */
        if (!ALL_FLAG && entry->d_name[0] == '.') {
            continue; /* Skip to the next iteration */
        }

        /* Dynamically allocate memory for each entry's name and inode */
        file_list = realloc(file_list, sizeof(struct file_info) * (count + 1));
        /* Check if realloc was successful */
        if (file_list == NULL) {
            perror("realloc failed"); /* Print error message */
            exit(EXIT_FAILURE); /* Exit the program */
        }

        file_list[count].name = strdup(entry->d_name); /* Copy the directory name */
        /* Check if strdup was successful */
        if (file_list[count].name == NULL) {
            perror("strdup failed"); /* Print error message */
            exit(EXIT_FAILURE); /* Exit the program */
        }

        file_list[count].inode = entry->d_ino; /* Store inode number */

        int len = strlen(entry->d_name); /* Get the length of the file name */
        /* Update maxlen if the current file name is longer and ONE_FLAG is not set */
        if (len > maxlen && !ONE_FLAG) {
            maxlen = len; /* Track the longest file name length */
        }

        /* Create the full path to the file */
        char filepath[512]; /* Buffer to hold the full file path */
        snprintf(filepath, sizeof(filepath), "%s/%s", dir, entry->d_name); /* Build the file path */

        /* Get the file status */
        if (lstat(filepath, &file_stat) == -1) {
            perror("lstat"); /* Print error message */
            continue; /* Skip to the next iteration */
        }

        /* Add the file's block size to the total */
        total_blocks += file_stat.st_blocks;

        /* Determine the color based on the file type */
        if (S_ISDIR(file_stat.st_mode) && !F_FLAG) {
            file_list[count].CHOOSEN_COLOR = BLUE; /* Directory in blue */
        } else if (S_ISLNK(file_stat.st_mode) && !F_FLAG) {
            file_list[count].CHOOSEN_COLOR = CYAN; /* Symbolic link in cyan */
            ssize_t len = readlink(filepath, symbolic_link_path, sizeof(symbolic_link_path) - 1); /* Read symbolic link */
            if (len == -1) {
                perror("readlink"); /* Print error message */
            }
            symbolic_link_path[len] = '\0'; /* Null-terminate the path */
        } else if ((file_stat.st_mode & S_IXUSR) && !F_FLAG) {
            file_list[count].CHOOSEN_COLOR = GREEN; /* Executable in green */
        } else {
            file_list[count].CHOOSEN_COLOR = RESET; /* Regular file, no color */
        }

        count++; /* Increment the file count */
    }

    closedir(dp); /* Close the directory */

    /* If there are files to display */
    if (count > 0) {
        /* If F_FLAG is not set, update global_dir to the current directory */
        if (!F_FLAG) {
            global_dir = dir; /* Update the global directory variable */

            /* Sort the files */
            qsort(file_list, count, sizeof(struct file_info), cmpstringp);
        }

        /* If L_FLAG is set, print total blocks and detailed information */
        if (L_FLAG) {
            /* Print the total block size in 512-byte blocks */
            printf("total %d\n", total_blocks / 2); /* Dividing by 2 because st_blocks is in 512-byte blocks */

            /* Loop through each file and print detailed info */
            for (int i = 0; i < count; i++) {
                if (I_FLAG) {
                    printf("%-8lu ", file_list[i].inode); /* Print inode with padding */
                }
                /* Now pass the file name from the file_list and directory for detailed info printing */
                print_detailed_info(dir, file_list[i].name, i);
            }
        } else {
            /* Choose between vertical or horizontal printing based on ONE_FLAG */
            if (ONE_FLAG) {
                VERTICAL_PRINTF_FUN(count); /* Print files vertically */
            } else {
                HORIZONTAL_PRINTF_FUN(count, maxlen); /* Print files horizontally */
            }
        }

        /* Free the allocated memory for each name in file_list */
        for (int i = 0; i < count; i++) {
            free(file_list[i].name); /* Free each file name */
        }

        free(file_list); /* Free the array of file_info structures */
    }
}