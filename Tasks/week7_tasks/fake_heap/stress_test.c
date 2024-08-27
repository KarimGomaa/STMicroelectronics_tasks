#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "std_types.h"
#include "my_heap.h"

#define NUM_ALLOCS 1000
#define MAX_SIZE 10000
#define MAX_ITERATIONS 1000

void random_alloc_free_test() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        
        // Randomly choose between allocation, reallocation, and freeing
        int operation = rand() % 3;
        size_t size = (size_t)(rand() % MAX_SIZE) + 1;

        switch (operation) {
            case 0: // Allocate new memory
                if (pointers[index] == NULL) {
                    if (rand() % 2) {
                        // Use HmmAlloc
                        pointers[index] = HmmAlloc(size);
                        if (pointers[index] != NULL) {
                            printf("Allocated memory of size %zu at address %p using HmmAlloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using HmmAlloc\n", size);
                        }
                    } else {
                        // Use HmmCalloc
                        pointers[index] = HmmCalloc(1, size); // Allocate memory for an array of 1 element of size 'size'
                        if (pointers[index] != NULL) {
                            printf("Allocated zero-initialized memory of size %zu at address %p using HmmCalloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using HmmCalloc\n", size);
                        }
                    }
                } else {
                    size_t newSize = (size_t)(rand() % MAX_SIZE) + 1;
                    printf("Mt5fsh y 3m HmmRealloc May Cause Segmentation Fault beacause of garbage run the test Again \n");
                    void* newPointer = HmmRealloc(pointers[index], newSize);
                    if (newPointer != NULL) {
                        printf("Reallocated memory to size %zu at address %p (old address was %p) using HmmRealloc\n", newSize, newPointer, pointers[index]);
                        pointers[index] = newPointer;
                    } else {
                        fprintf(stderr, "Reallocation failed for new size %zu\n", newSize);
                    }
                }
                break;

            case 1: // Reallocate existing memory
                if (pointers[index] != NULL) {
                    size_t newSize = (size_t)(rand() % MAX_SIZE) + 1;
                    printf("Mt5fsh y 3m HmmRealloc May Cause Segmentation Fault beacause of garbage run the test Again \n");
                    void* newPointer = HmmRealloc(pointers[index], newSize);
                    if (newPointer != NULL) {
                        printf("Reallocated memory to size %zu at address %p (old address was %p) using HmmRealloc\n", newSize, newPointer, pointers[index]);
                        pointers[index] = newPointer;
                    } else {
                        fprintf(stderr, "Reallocation failed for new size %zu\n", newSize);
                    }
                } else {
                    if (rand() % 2) {
                        // Use HmmAlloc
                        pointers[index] = HmmAlloc(size);
                        if (pointers[index] != NULL) {
                            printf("Allocated memory of size %zu at address %p using HmmAlloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using HmmAlloc\n", size);
                        }
                    } else {
                        // Use HmmCalloc
                        pointers[index] = HmmCalloc(1, size); // Allocate memory for an array of 1 element of size 'size'
                        if (pointers[index] != NULL) {
                            printf("Allocated zero-initialized memory of size %zu at address %p using HmmCalloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using HmmCalloc\n", size);
                        }
                    }
                }
                break;

            case 2: // Free existing memory
                if (pointers[index] != NULL) {
                    printf("Freeing memory at address %p\n", pointers[index]);
                    HmmFree(pointers[index]);
                    pointers[index] = NULL;
                }else {
                    if (rand() % 2) {
                        // Use HmmAlloc
                        pointers[index] = HmmAlloc(size);
                        if (pointers[index] != NULL) {
                            printf("Allocated memory of size %zu at address %p using HmmAlloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using HmmAlloc\n", size);
                        }
                    } else {
                        // Use HmmCalloc
                        pointers[index] = HmmCalloc(1, size); // Allocate memory for an array of 1 element of size 'size'
                        if (pointers[index] != NULL) {
                            printf("Allocated zero-initialized memory of size %zu at address %p using HmmCalloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using HmmCalloc\n", size);
                        }
                    }
                }
                break;
        }
    }
    
    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            HmmFree(pointers[i]);
            pointers[i] = NULL;
        }
    }
}

int main() {
    printf("Starting random allocation, reallocation, and deallocation test...\n");
    random_alloc_free_test();
    printf("Test complete.\n");
    return 0;
}
