 /******************************************************************************
 *
 * File Name: my_heap.c
 *
 * Description: Source file Contain the implementation of My Heap
 *
 * Author: Karim Gomaa
 *
 *******************************************************************************/


#include <stdio.h>
#include <unistd.h>  /*For sleep function to test the heap*/
#include "std_types.h"
#include "my_heap.h"

static uint8 MY_HEAP[HEAP_SIZE];  /* Simulated heap area */
static uint8 *program_break = MY_HEAP;  /* Pointer to simulate the program break */
static BlockMeta *first_free_block = NULL;  /* Pointer to the first free block */
static BlockMeta *last_free_block = NULL;  /* Pointer to the first free block */
static uint8 exit_sleep=0;  /* For debugging */


/*
 * Description:
 * 1. Recalculates and updates the total amount of free space in the heap.
 * 2. Traverses the list of free blocks to sum the sizes of all free blocks.
 * 3. Updates any relevant global or static variables that track the total free space.
 * 4. Ensures that the free space information is accurate and up-to-date, which helps in efficient memory allocation and management.
 * 5. Typically called after operations that modify the free blocks, such as freeing memory or merging adjacent free blocks, to ensure the free space information remains consistent.
 *
 * Parameters:
 * - This function does not take any parameters.
 *
 * Return Value:
 * - This function does not return a value. It performs internal updates to maintain accurate free space information.
 */
static void fun_SumAllFreeSpaces()
{
    BlockMeta *sum_all_free_spaces = first_free_block;
    BlockMeta *next_ptr;
    while(sum_all_free_spaces != NULL )
    {
         next_ptr = sum_all_free_spaces->next;

        // Check if current block is adjacent to the next block
        if((uint8 *)sum_all_free_spaces + sum_all_free_spaces->size == (uint8 *)next_ptr)
        {
            // Merge the two blocks
            sum_all_free_spaces->size += next_ptr->size;
            sum_all_free_spaces->next = next_ptr->next;

            // Adjust the previous pointer of the next-next block if it exists
            if(next_ptr->next != NULL)
            {
                BlockMeta *next_next_ptr=next_ptr->next;
                next_next_ptr->prev = sum_all_free_spaces;
            } else {
                last_free_block = sum_all_free_spaces;
                break;
            }

            continue;  // Recheck the current block after merging
        }
        // Move to the next block if no merge was done
        sum_all_free_spaces = next_ptr;
    } 
}

/* Function to Update the allocated block and return it to the user */
static void *fun_SetUpTheReturnedPointer(BlockMeta *checking_free_place,uint64 size)
{
    /* Set up the metadata for the allocated block */
    BlockMeta *allocated_block = checking_free_place;
    allocated_block->size = size;
    allocated_block->next = NULL;
    allocated_block->prev = NULL;

    /* Return the pointer to the allocated memory (after the metadata) */
    return (void *)RETURN(allocated_block);
}

/*
 * Description:
 * 1. Inserts a new free block into the free list at the end of the list, following the current free block.
 * 2. The function takes a pointer to a pointer (`checking_free_place_ptr`) which points to the current free block, and the size of the new free block to be inserted.
 * 3. Updates the metadata of the newly created free block, setting its size and pointers (`next` and `prev`) to reflect its position in the list.
 * 4. Adjusts the pointers of adjacent free blocks to include the new block in the free list:
 *    - If there is no previous block, sets the new block as the first free block.
 *    - Updates the `next` pointer of the previous block and the `prev` pointer of the next block if they exist.
 * 5. Ensures that the new block is properly linked into the free list, maintaining the correct order and consistency of the free blocks.
 * 6. Handles cases where the heap's program break needs to be adjusted to accommodate the new free block.
 *
 * Parameters:
 * - `checking_free_place_ptr`: A pointer to a pointer to the current free block where the new block will be inserted after.
 * - `size`: The size of the new free block to be inserted, in bytes.
 *
 * Return Value:
 * - This function does not return a value. It performs operations to insert the new block into the free list and update relevant metadata.
 */
static void fun_InsertNodeFromTheEnd(BlockMeta **checking_free_place_ptr, uint64 size)
{
    /* Dereference the pointer to get the current free block */
    BlockMeta *checking_free_place = *checking_free_place_ptr;

    /* Check if the program break exceeds the end of the heap */
    if (program_break > MY_HEAP + HEAP_SIZE) 
    {
        /* If it does, adjust the program break to the end of the heap */
        program_break = MY_HEAP + HEAP_SIZE;
        /* Reset the pointer to the first free block */
        first_free_block = NULL;
        return;
    }

    /* Set up the metadata for the next free block */
    /* Calculate the address for the next free block */
    BlockMeta *next_free_space = (BlockMeta *)((uint8 *)checking_free_place + METADATA_SIZE + size);
    
    /* Set the size of the new free block */
    next_free_space->size = (uint64)(program_break - (uint8 *)next_free_space);
    /* Initialize the next pointer to NULL */
    next_free_space->next = NULL;
    /* Set the previous pointer to the current free block's previous pointer */
    next_free_space->prev = checking_free_place->prev;

    /*Updating the last free pointer*/
    last_free_block = next_free_space;

    /* Check if the previous free block exists */
    if (checking_free_place->prev != NULL)
    {
        /* Update the previous free block's next pointer to the new free block */
        BlockMeta *prev_free_space = checking_free_place->prev;
        prev_free_space->next = next_free_space;
        if(prev_free_space == NULL)
        {
            first_free_block = prev_free_space;
        }
    }
    else
    {
        /* If there is no previous free block, update the first free block pointer */
        first_free_block = next_free_space;
    } 
}

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
void *HmmAlloc(uint64 size) {

    
    if(!exit_sleep)
    {
    sleep(1);
    exit_sleep=1;
    }

    /*Align the entered size to 8 */
    size = (( size + 7) / 8) * 8;

    /*Some programs need a pointer to be returned when allocating size = 0*/
    if(size == 0 )
    {
        size = 8;
    }

    /* Used when entering HmmAlloc for the first time */
    if (program_break == MY_HEAP) 
    {
        /* Set up the metadata for the allocated block */
        BlockMeta *allocated_block = (BlockMeta *)program_break;
        allocated_block->size = size;
        allocated_block->next = NULL;
        allocated_block->prev = NULL;

        /* Move the program break forward by the size of the allocation + PROGRAM_BREAK_EXTEND + BlockMeta size to avoid overhead */
        program_break = program_break + METADATA_SIZE + size + PROGRAM_BREAK_EXTEND;

        /* Set up the first free block, which is located right after the allocated block */
        first_free_block = (BlockMeta *)(program_break - PROGRAM_BREAK_EXTEND);
        first_free_block->size = (uint64)(program_break - (uint8 *)first_free_block); /* Calculate the full free space available */
        first_free_block->next = NULL;  /* No next free block for now */
        first_free_block->prev = NULL;  /* No previous free block for now */

        /*update the last free block*/
        last_free_block = first_free_block;

        /* Return the pointer to the allocated memory (after the metadata) */
        return (void *)RETURN(allocated_block);
    }
    else
    {
        /* Check if there is no free block available */
        if (first_free_block == NULL) {
            return NULL;
        }

        /* Start checking from the first free block */
        BlockMeta *checking_free_place = (BlockMeta *)first_free_block; 
        while (1)
        {
            /* Check if there are no more free blocks to check */
            if (checking_free_place == NULL)
            {
                return NULL;
            }

            /* Check if the current block is large enough to accommodate the requested size */
            if ((checking_free_place->size > size + METADATA_SIZE) && 
                ((uint8 *)checking_free_place + size + METADATA_SIZE <= program_break))
            {
                /* Calculate the remaining size after allocation */
                uint64 checking_size_cases = checking_free_place->size - (size + METADATA_SIZE);

                /* Handle case where the current block is the last free block */
                if (checking_free_place->next == NULL)
                {
                    if (checking_size_cases <= METADATA_SIZE)
                    {
                        /* Extend the program break to accommodate the new allocation */
                        program_break = program_break + size + METADATA_SIZE + PROGRAM_BREAK_EXTEND;
                    }

                    /* Function to insert a node from the end of the array */
                    fun_InsertNodeFromTheEnd(&checking_free_place, size);
                }
                /* Handle case where there is a next block and remaining space is enough for a new block */
                else if (checking_free_place->next != NULL && checking_size_cases > METADATA_SIZE)
                {
                    /* Set up the metadata for the next free block */
                    BlockMeta *next_free_space = (BlockMeta *)((uint8 *)checking_free_place + METADATA_SIZE + size);
                    next_free_space->size = checking_size_cases;
                    next_free_space->next = checking_free_place->next;
                    next_free_space->prev = checking_free_place->prev;
                    
                    /*the next now free block after checking free space now pointing to next free space*/
                    BlockMeta *checking_free_place_next=checking_free_place->next;
                    checking_free_place_next->prev = next_free_space;

                    /* Update the previous free block's next pointer or the first free block pointer */
                    if (checking_free_place->prev != NULL)
                    {
                        BlockMeta *prev_free_space = checking_free_place->prev;
                        prev_free_space->next = next_free_space;
                    }
                    else
                    {
                        first_free_block = next_free_space;
                    }
                }
                /* Handle case where there is a next block and remaining space is not enough for a new block */
                else if (checking_free_place->next != NULL && checking_size_cases <= METADATA_SIZE)
                {
                    if (checking_free_place->prev == NULL)
                    {
                        first_free_block = checking_free_place->next;
                        first_free_block->prev = NULL;
                    }
                    else
                    {
                        /* Update the previous and next pointers */
                        BlockMeta *next_free_space = checking_free_place->next;
                        BlockMeta *prev_free_space = checking_free_place->prev;
                        next_free_space->prev = prev_free_space;
                        prev_free_space->next = next_free_space;
                    }
                }

                /* Adjust the size to account for any remaining space that was merged */
                if (checking_size_cases <= METADATA_SIZE)
                {
                    size = size + checking_size_cases;
                }

                /* Function to set up the metadata and return a pointer after the metadata of type void * */
                void* Return_Pointer = fun_SetUpTheReturnedPointer(checking_free_place, size);
                return Return_Pointer;
            }
            /* Handle case where the current block is too small and we are at the end of the heap */
            else if ((checking_free_place->size < size + METADATA_SIZE) && 
                     ((uint8 *)checking_free_place + size + METADATA_SIZE > program_break) &&
                     ((uint8 *)checking_free_place + size + METADATA_SIZE < MY_HEAP + HEAP_SIZE) &&
                     (checking_free_place->next == NULL) )
            {
                /* Extend the program break to accommodate the new allocation */
                program_break = program_break + size + METADATA_SIZE + PROGRAM_BREAK_EXTEND;

                /* Function to insert a node from the end of the array */
                fun_InsertNodeFromTheEnd(&checking_free_place, size);

                /* Function to set up the metadata and return a pointer after the metadata of type void * */
                void* Return_Pointer = fun_SetUpTheReturnedPointer(checking_free_place, size);
                return Return_Pointer;
            }
            /* Handle case where the heap is full and cannot accommodate the requested size */
            else if ((checking_free_place->size < size + METADATA_SIZE) &&
                     ((uint8 *)checking_free_place + size + METADATA_SIZE > program_break) &&
                     ((uint8 *)checking_free_place + size + METADATA_SIZE > MY_HEAP + HEAP_SIZE))
            {
                return NULL;
            }

            /* Move to the next free block */
            checking_free_place = checking_free_place->next;
        } 
    }
}


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
void HmmFree(void *ptr) {

    /* Check if the provided pointer is NULL */
    if (ptr == NULL)
    {   
        return ;
    }

    /* Calculate the metadata pointer from the provided memory pointer */
    BlockMeta *free_this_ptr = (BlockMeta *)((uint8 *)ptr - METADATA_SIZE);

    /* Handle the case where there are no free blocks yet */
    if (first_free_block == NULL)
    {
        /* Update metadata size to include the freed space */
        free_this_ptr->size = free_this_ptr->size + METADATA_SIZE;
        free_this_ptr->next = NULL;
        free_this_ptr->prev = NULL;
        /* Set the first free block to the newly freed block */
        first_free_block = free_this_ptr;
        last_free_block = free_this_ptr;
        return ;
    }

    /* Initialize pointer to traverse the free blocks list */
    BlockMeta *where_is_this_ptr = (BlockMeta *)first_free_block;

    /* Loop until the current free block is after the block to be freed */
    while (where_is_this_ptr < free_this_ptr && where_is_this_ptr->next != NULL)
    {
        where_is_this_ptr = where_is_this_ptr->next;
        if(where_is_this_ptr == free_this_ptr )
        {
            return ; 
        }
    }

    /* Case when the block to be freed is after the current block in the list */
    if (where_is_this_ptr < free_this_ptr)
    {
        /* Update metadata size to include the freed space */
        free_this_ptr->size = free_this_ptr->size + METADATA_SIZE;
        free_this_ptr->next = NULL;
        free_this_ptr->prev = where_is_this_ptr;
        /* Insert the freed block at the end of the list */
        where_is_this_ptr->next = free_this_ptr;

        /*update the last free block*/
        last_free_block = free_this_ptr;

        /* Update the list of free spaces */
        fun_SumAllFreeSpaces();
        return ;
    }
 
    /* Case when the block to be freed is in the middle of the list */
    free_this_ptr->size += METADATA_SIZE;
    free_this_ptr->next = where_is_this_ptr;
    free_this_ptr->prev = where_is_this_ptr->prev;

    /* Update the previous block's next pointer or the first free block pointer */
    if (where_is_this_ptr->prev != NULL)
    {
        BlockMeta *where_is_this_ptr_prev=where_is_this_ptr->prev;
        where_is_this_ptr_prev->next = free_this_ptr;
    }
    else
    {
        first_free_block = free_this_ptr;
    }

    /* Update the current block's previous pointer */
    where_is_this_ptr->prev = free_this_ptr;

    /* Update the list of free spaces */
    fun_SumAllFreeSpaces();
    return;
}


/*
 * Description:
 *   Custom implementation of calloc that allocates memory for an array and initializes all bytes to zero.
 *   The function aligns the total memory size to 8 bytes for better performance on some architectures.
 *
 * Parameters:
 *   - num_elements: The number of elements to allocate memory for.
 *   - size_of_each_element: The size of each element in bytes.
 *
 * Returns:
 *   - A pointer to the allocated memory if successful.
 *   - NULL if memory allocation fails.
 */
void *HmmCalloc(uint32 num_elements, uint32 size_of_each_element) {
    /* Calculate the total size required */
    uint64 total_size = num_elements * size_of_each_element;

    /* Align the total size to 8 bytes for optimal memory alignment */
    total_size = ((total_size + 7) / 8) * 8;

    /*If size equal to zero send a small place that can be freed later*/
    if(total_size == 0)
    {
        total_size = 8;
    }

    /* Allocate memory using custom HmmAlloc function */
    void *allocated_ptr = HmmAlloc(total_size);

    /* Check if memory allocation was successful */
    if (allocated_ptr == NULL) {
        return NULL; /* Memory allocation failed, return NULL */
    }

    /* Cast the pointers to uint8 for byte-wise copying */
    uint8 *access_allocated_ptr = (uint8 *)(allocated_ptr);

    /* Initialize the allocated memory to zero  */
    for (int i = 0; i < total_size; i++)
    {   
        access_allocated_ptr[i] = 0;
    }

    return allocated_ptr; /* Return the pointer to the allocated memory */
}


/*
 * Description:
 *   Custom implementation of realloc that reallocates a memory block to a new size.
 *   If the new size is larger, it copies the existing data to the newly allocated memory.
 *   If the pointer is NULL, it behaves like malloc.
 *   If the size is 0, it frees the memory.
 *
 * Parameters:
 *   - ptr: The pointer to the previously allocated memory block.
 *   - size: The new size in bytes for the memory block.
 *
 * Returns:
 *   - A pointer to the newly allocated memory block if successful.
 *   - NULL if memory allocation fails or if size is 0 and ptr is not NULL.
 */
void *HmmRealloc(void *ptr, uint64 size)
{
    void *allocated_ptr;

    /* If the input pointer is NULL, allocate new memory */
    if (ptr == NULL)
    {
        allocated_ptr = HmmAlloc(size);
        return allocated_ptr;   
    }

    /* If the size is 0, free the memory and return NULL */
    if (size == 0)
    {
        HmmFree(ptr);
        return NULL;
    }

    /* Align the entered size to 8 bytes for optimal memory alignment */
    size = ((size + 7) / 8) * 8;

    /* Allocate new memory of the specified size */
    allocated_ptr = HmmAlloc(size);
    if (allocated_ptr == NULL) {
        return NULL;  /* Allocation failed, return NULL  */
    }

    /* Get the size of the existing memory block using the metadata */
    BlockMeta *get_size_of_ptr = (BlockMeta *)((uint8 *)ptr - METADATA_SIZE);

    /* Determine the smaller of the old and new sizes for copying */
    uint64 looping_size = (get_size_of_ptr->size < size) ? get_size_of_ptr->size : size;

    /* Cast the pointers to uint8 for byte-wise copying */
    uint8 *copy_to_allocated_ptr = (uint8 *)(ptr);
    uint8 *access_allocated_ptr = (uint8 *)(allocated_ptr);

    /* Copy the data from the old memory block to the new one */
    for (int i = 0; i < looping_size; i++)
    {   
        access_allocated_ptr[i] = copy_to_allocated_ptr[i];
    }

    /* Free the old memory block */
    HmmFree(ptr);

    /* Return the pointer to the newly allocated memory block */
    return allocated_ptr;
}


/********************************************** To Be tested instead of real Heap* *********************************************/

void *malloc(uint64 size)
{
    return  HmmAlloc(size);
}

void free(void *ptr)
{
    HmmFree(ptr);
}

void *calloc(uint64 nmemb,uint64 size)
{
    return HmmCalloc(nmemb,size);
}

void *realloc(void *ptr, uint64 size)
{
    return HmmRealloc(ptr, size);
}

/************************************************************* END *************************************************************/
