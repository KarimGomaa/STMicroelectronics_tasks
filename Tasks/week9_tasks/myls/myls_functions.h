/******************************************************************************
 *
 * File Name: myls_fuctions.h
 *
 * Description: Header file for myls_functions.c
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#ifndef MYLS_FUNCTIONS_H
#define MYLS_FUNCTIONS_H

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

typedef enum {
    RESET,
    BLUE,
    GREEN,
    CYAN
} COLOR;

struct file_info{
    char *name;
    long unsigned int inode;
	COLOR CHOOSEN_COLOR;
};

 /*******************************************************************************
 *                           Extern Global Variable                            *
 *******************************************************************************/

extern char ALL_FLAG;              // show all files (not hidden and hidded) Flag
extern char ONE_FLAG;              // One per line flag
extern char L_FLAG;                // Long List Format flag
extern char F_FLAG;                // Don't sort flag
extern char T_FLAG;                // Sort by Modification time
extern char U_FLAG;                // Sort by Access Time
extern char C_FLAG;                // Sort by last Status time
extern char I_FLAG;                // Display i node
extern char D_FLAG;                // Display Directory itself

 /*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/* 
 * Function to print an error message for invalid options. 
 * This function doesn't take any parameters and prints an error message
 * guiding the user on how to use the correct command-line options.
 */
void PRINT_ERROR_FUN();


/* 
 * Comparison function for qsort to compare file paths.
 * It checks the sorting flags (T_FLAG, U_FLAG, C_FLAG) and compares 
 * file times accordingly. If no flags are set, it compares file names alphabetically.
 */
int cmpstringp(const void *p1, const void *p2);


/* 
 * Function: do_ls 
 * -----------------
 * This function lists the contents of a specified directory, printing 
 * details based on various flags that control the output format.
 * 
 * Parameters:
 *   - char *dir: the directory path to list.
 */
void do_ls(char *dir);

#endif // MYLS_FUNCTIONS_H