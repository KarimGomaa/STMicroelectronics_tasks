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
 * Description:
 * 1. Allocates a block of memory of the specified size from the custom heap.
 * 2. If the heap is empty (i.e., this is the first allocation), it initializes the heap with the allocated block and sets up the first free block immediately after it.
 * 3. If the heap is not empty, it searches through the list of free blocks to find a suitable block that can accommodate the requested size.
 * 4. If a suitable block is found:
 *    - It adjusts the size of the block and potentially splits it if there is leftover space.
 *    - Updates the metadata for the free list to maintain consistency.
 * 5. If no suitable block is found, it extends the heap if there is enough space and adjusts the free block list accordingly.
 * 6. Returns a pointer to the start of the allocated memory block, which is offset from the metadata.
 * 7. If allocation fails (e.g., due to insufficient memory), it returns `NULL`.
 *
 * Parameters:
 * - `size`: The size of the memory block to allocate, in bytes.
 *
 * Return Value:
 * - Returns a pointer to the allocated memory block, or `NULL` if the allocation fails.
 */
void *HmmAlloc(uint64 size);


/*
 * Description:
 * 1. Frees a previously allocated block of memory, making it available for future allocations.
 * 2. Checks if the provided pointer is `NULL`. If so, it prints an error message and returns without performing any operations.
 * 3. Calculates the metadata pointer for the block to be freed based on the given pointer.
 * 4. Handles the case where the free list is empty by initializing it with the freed block.
 * 5. If the free list is not empty:
 *    - Traverses the list to find the appropriate position for the freed block.
 *    - Inserts the freed block into the correct position in the free list while maintaining proper pointers.
 * 6. Merges adjacent free blocks if necessary, updating the metadata to reflect the new size of the combined free space.
 * 7. Calls an auxiliary function to update the free spaces list, ensuring the heap’s free memory is accurately represented.
 *
 * Parameters:
 * - `ptr`: A pointer to the memory block to be freed. This pointer should have been previously allocated using `HmmAlloc`.
 *
 * Return Value:
 * - This function does not return a value. It performs operations to free memory and update the free list.
 */
void HmmFree(void *ptr);


#endif /* MY_HEAP_H */