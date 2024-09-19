/******************************************************************************
 *
 * File Name: myls_main.c
 *
 * Description: Source file cantaining the main function of myls program
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
 * Function: main 
 * ---------------
 * The entry point of the program that parses command-line options,
 * sorts directories, and calls the do_ls function to list directory contents.
 *
 * Parameters:
 *   - int argc: the number of command-line arguments.
 *   - char *argv[]: an array of command-line arguments.
 *
 * Returns:
 *   - int: exit status of the program.
 */
int main(int argc, char *argv[])
{
    int opt; /* Variable to hold the option returned by getopt */

    /* Parse command-line options using getopt */
    while ((opt = getopt(argc, argv, ":latucifd1")) != -1) {
        /* Switch statement to handle each option */
        switch (opt) {
        case 'l':
            L_FLAG = 1; /* Set L_FLAG to indicate long listing format */
            break; /* Exit the switch case */
        case 'a':
            ALL_FLAG = 1; /* Set ALL_FLAG to include hidden files */
            break; /* Exit the switch case */
        case 't':
            T_FLAG = 1; /* Set T_FLAG to sort by time */
            U_FLAG = 0; /* Reset U_FLAG */
            C_FLAG = 0; /* Reset C_FLAG */
            break; /* Exit the switch case */
        case 'u':
            T_FLAG = 0; /* Reset T_FLAG */
            U_FLAG = 1; /* Set U_FLAG to sort by access time */
            C_FLAG = 0; /* Reset C_FLAG */
            break; /* Exit the switch case */
        case 'c':
            T_FLAG = 0; /* Reset T_FLAG */
            U_FLAG = 0; /* Reset U_FLAG */
            C_FLAG = 1; /* Set C_FLAG to sort by status change time */
            break; /* Exit the switch case */
        case 'i':
            I_FLAG = 1; /* Set I_FLAG to display inode numbers */
            break; /* Exit the switch case */
        case 'f':
            F_FLAG = 1; /* Set F_FLAG to force output without sorting */
            ALL_FLAG = 1; /* Set ALL_FLAG to include hidden files */
            L_FLAG = 0; /* Reset L_FLAG */
            break; /* Exit the switch case */
        case 'd':
            D_FLAG = 1; /* Set D_FLAG to display directory itself */
            break; /* Exit the switch case */
        case '1':
            ONE_FLAG = 1; /* Set ONE_FLAG to display one entry per line */
            break; /* Exit the switch case */
        case '?':
            PRINT_ERROR_FUN(); /* Print error for unknown option */
            return 1; /* Return with an error code */
            break; /* Exit the switch case */
        default:
            printf("Unexpected case in switch() \n"); /* Handle unexpected case */
            return -1; /* Return with an error code */
        }
    }

    /* Check if there are any additional command-line arguments */
    if (optind < argc) {
        int i = optind; /* Initialize index to the first non-option argument */
        /* Sort the directory names passed as arguments */
        qsort(&argv[i], argc - optind, sizeof(char *), cmpstringp);
        /* Loop through the sorted directories */
        while (i < argc) {
            printf("Kemo's ls listing of %s:\n", argv[i]); /* Print the directory name */
            do_ls(argv[i]); /* Call do_ls to list contents of the directory */
            i++; /* Increment index */
            /* Print a newline if there are more directories to list */
            if (i < argc) {
                printf("\n");
            }
        }
    } else {
        /* If no directories are specified, list the current directory */
        do_ls("."); /* Call do_ls for the current directory */
    }

    return 0; /* Return success */
}