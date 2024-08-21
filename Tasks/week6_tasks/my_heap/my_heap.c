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
#include "std_types.h"
#include "my_heap.h"

static uint8 MY_HEAP[HEAP_SIZE];  /* Simulated heap area */
static uint8 *program_break = MY_HEAP;  /* Pointer to simulate the program break */
static BlockMeta *first_free_block = NULL;  /* Pointer to the first free block */

static void fun_SumAllFreeSpaces()
{
    BlockMeta *sum_all_free_spaces = first_free_block;
    int i=0;
    while(sum_all_free_spaces != NULL && i != 1000 )
    {
        BlockMeta *next_ptr = sum_all_free_spaces->next;
        i++;
        // Check if current block is adjacent to the next block
        if((uint8 *)sum_all_free_spaces + sum_all_free_spaces->size == (uint8 *)next_ptr)
        {
            // Merge the two blocks
            sum_all_free_spaces->size += next_ptr->size;
            sum_all_free_spaces->next = next_ptr->next;

            // Adjust the previous pointer of the next-next block if it exists
            if(next_ptr->next != NULL)
            {
                next_ptr->next->prev = sum_all_free_spaces;
            }

            continue;  // Recheck the current block after merging
        }
        // Move to the next block if no merge was done
        sum_all_free_spaces = next_ptr;
    } 
}

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

static void fun_InsertNodeFromTheEnd(BlockMeta **checking_free_place_ptr,uint64 size)
{
    BlockMeta *checking_free_place = *checking_free_place_ptr;

    if(program_break > MY_HEAP + HEAP_SIZE ) 
    {
        program_break = MY_HEAP + HEAP_SIZE;
        first_free_block = NULL;
        return ;
    }

    /* Set up the metadata for the next free block */
    BlockMeta *next_free_space=(BlockMeta *)( (uint8 *)checking_free_place + METADATA_SIZE + size );
    next_free_space->size = (uint64)(program_break - (uint8 *)next_free_space );
    next_free_space->next = NULL;
    next_free_space->prev = checking_free_place->prev;
    if(checking_free_place->prev != NULL)
    {
        BlockMeta *prev_free_space=checking_free_place->prev;
        prev_free_space->next = next_free_space;     
    }
    else
    {
        first_free_block=next_free_space;
    } 
}

void *HmmAlloc(uint64 size) {

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
        first_free_block->size = (uint64)(program_break - (uint8 *)first_free_block);   /*we are calculating the full free space available right now and we will check for metadata_size in the else case*/
        first_free_block->next = NULL;  /* No next free block for now */
        first_free_block->prev = NULL;  /* No previous free block for now */

        /* Return the pointer to the allocated memory (after the metadata) */
        return (void *)RETURN(allocated_block);
    }
    else
    {
        
        if(first_free_block == NULL){
            printf("No Free Space try to free Some spaces at first \n");
            return NULL;
        }

        BlockMeta *checking_free_place=(BlockMeta *) first_free_block;
        while(1)
        {
            if(checking_free_place == NULL)
            {
                printf("Sorry, Can't Allocate memory in the heap \n");
                return NULL;
            }

            if(  (checking_free_place->size > size + METADATA_SIZE ) &&  ( (uint8 *)checking_free_place + size + METADATA_SIZE < program_break)  )
            {
                uint64 checking_size_cases=checking_free_place->size - (size + METADATA_SIZE);

                if(checking_free_place->next == NULL)
                { 
                    if(checking_size_cases < METADATA_SIZE  )
                    {
                        /* Move the program break forward by the size of the allocation + PROGRAM_BREAK_EXTEND */
                        program_break = program_break + size + METADATA_SIZE + PROGRAM_BREAK_EXTEND;  
                    }

                    /*function to insert node form the end of the array */
                    fun_InsertNodeFromTheEnd(&checking_free_place,size);
                }
                else if(checking_free_place->next != NULL && checking_size_cases > METADATA_SIZE )
                {
                    /* Set up the metadata for the next free block */
                    BlockMeta *next_free_space=(BlockMeta *)( (uint8 *)checking_free_place + METADATA_SIZE + size );
                    next_free_space->size = checking_size_cases;
                    next_free_space->next = checking_free_place->next;
                    next_free_space->prev = checking_free_place->prev;
                    if(checking_free_place->prev != NULL)
                    {
                        BlockMeta *prev_free_space=checking_free_place->prev;
                        prev_free_space->next = next_free_space;
                    }else{
                        first_free_block=next_free_space;
                        if(next_free_space != NULL)
                        {
                            next_free_space->next->prev = next_free_space;
                        }
                    }
                }
                else if(checking_free_place->next != NULL && checking_size_cases < METADATA_SIZE  )
                {
                    if(checking_free_place->prev == NULL)
                    {
                        first_free_block=checking_free_place->next;
                    }
                    else{
                        /* Set up the metadata for the next free block */
                        BlockMeta *next_free_space=checking_free_place->next;
                        BlockMeta *prev_free_space=checking_free_place->prev;
                        next_free_space->prev = prev_free_space;
                        prev_free_space->next = next_free_space;
                    }
                
                }

                if(checking_size_cases < METADATA_SIZE )
                {
                    size = size + checking_size_cases;
                }
                /*Function to set up the meta data and return a pointer after the meta data of type void *    */
                void* Return_Pointer = fun_SetUpTheReturnedPointer(checking_free_place,size);
                return Return_Pointer;

            }
            else if( (checking_free_place->size < size + METADATA_SIZE) && (( (uint8 *)checking_free_place + size + METADATA_SIZE > program_break) && ( (uint8 *)checking_free_place + size + METADATA_SIZE < MY_HEAP + HEAP_SIZE)) && (checking_free_place->next == NULL) )
            {
                /* Move the program break forward by the size of the allocation + PROGRAM_BREAK_EXTEND */
                program_break = program_break + size + METADATA_SIZE + PROGRAM_BREAK_EXTEND;

                /*function to insert node form the end of the array */
                fun_InsertNodeFromTheEnd(&checking_free_place,size);

                /*Function to set up the meta data and return a pointer after the meta data of type void *    */
                void* Return_Pointer = fun_SetUpTheReturnedPointer(checking_free_place,size);
                return Return_Pointer;

            }
            else if( (checking_free_place->size < size + METADATA_SIZE) && (( (uint8 *)checking_free_place + size + METADATA_SIZE > program_break) && ((uint8 *)checking_free_place + size + METADATA_SIZE > MY_HEAP + HEAP_SIZE)) )
            {
                printf("Sorry Heap is Full you can't Allocate any memory right now, Try free some memory before Allocating \n");
                return NULL;  /* No allocation was done as there is not a space in heap */
            }

            checking_free_place=checking_free_place->next;
        }
    }
}

void HmmFree(void *ptr) {

    if(ptr == NULL)
    {   
        printf("No pointer to free you have send a NULL pointer -_- \n");
        return;
    }

    BlockMeta *free_this_ptr=(BlockMeta *)( (uint8 *)ptr - METADATA_SIZE);
    if(first_free_block == NULL)
    {
        free_this_ptr->size= free_this_ptr->size + METADATA_SIZE ;
        free_this_ptr->next = NULL;
        free_this_ptr->prev = NULL;
        first_free_block = free_this_ptr;
        return ;
    }

    BlockMeta *where_is_this_ptr =(BlockMeta *)(first_free_block);

    /*Loop untill where is this pointer is after free this ptr*/
    while(where_is_this_ptr < free_this_ptr && where_is_this_ptr->next != NULL)
    {
        where_is_this_ptr = where_is_this_ptr->next;
    }

    if(where_is_this_ptr < free_this_ptr )
    {
        free_this_ptr->size = free_this_ptr->size + METADATA_SIZE ;
        free_this_ptr->next = NULL;
        free_this_ptr->prev = where_is_this_ptr;
        where_is_this_ptr->next = free_this_ptr;
        fun_SumAllFreeSpaces();
        return;
    }

    free_this_ptr->size += METADATA_SIZE;
    free_this_ptr->next = where_is_this_ptr;
    free_this_ptr->prev = where_is_this_ptr->prev;

    if (where_is_this_ptr->prev != NULL) {
        where_is_this_ptr->prev->next = free_this_ptr;
    } else {
        first_free_block = free_this_ptr;
    }

    where_is_this_ptr->prev = free_this_ptr;

   fun_SumAllFreeSpaces();
}