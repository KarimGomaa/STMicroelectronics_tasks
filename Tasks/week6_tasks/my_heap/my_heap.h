 /******************************************************************************
 *
 * File Name: my_heap.h
 *
 * Description: Header file for my_heap.c
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/

#ifndef MY_HEAP_H
#define MY_HEAP_H

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#include "std_types.h"

#define HEAP_SIZE                       (300000000)                             /* Define the 300 MB size of the simulated heap */
#define PROGRAM_BREAK_EXTEND            (125000)                                   /* Define a constant extended number (125 KB) for program break to avoid overheads */
#define METADATA_SIZE                   sizeof(BlockMeta)                       /* Size of Meta data which is 24 byte (8 for size and 8 for next pointer and 8 for prev pointer)*/
#define RETURN(ptr)                     ((uint8 *)ptr + sizeof(BlockMeta))      /* Defined to return a pointer after the Meta Data to be used by the user*/

typedef struct BlockMeta {
    uint64 size;               /* Size of the allocated block (8 Byte) */
    struct BlockMeta *next;    /* Pointer to the next free block (8 Byte) */
    struct BlockMeta *prev;    /* Pointer to the previous free block (8 Byte) */
} BlockMeta;

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/*
 * Description :
 * 1.
 */
void *HmmAlloc(uint64 size);

/*
 * Description :
 * 1.
 */
void HmmFree(void *ptr);

#endif /* MY_HEAP_H */