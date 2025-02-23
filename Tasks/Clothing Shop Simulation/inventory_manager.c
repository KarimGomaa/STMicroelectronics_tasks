#include <stdio.h>      /* Standard I/O operations */
#include <stdlib.h>     /* Standard library for memory management, process control, etc. */
#include <string.h>     /* String manipulation functions */
#include <sys/ipc.h>    /* Inter-process communication (IPC) keys */
#include <sys/shm.h>    /* Shared memory operations */
#include <sys/sem.h>    /* Semaphore operations */
#include <sys/stat.h>   /* File permissions and status functions */
#include <fcntl.h>      /* File control operations (used for FIFO pipes) */
#include <unistd.h>     /* System calls like open(), read(), write(), and close() */

#define SHM_KEY 5678  /* Shared memory key */
#define SEM_KEY 91011 /* Semaphore key */
#define MAX_ITEMS 15  /* Maximum number of inventory items */
#define FIFO_UPDATE "/tmp/inventory_update_fifo"  /* FIFO for real-time updates */

/* Structure to store item details */
typedef struct {
    char name[20];  /* Item name */
    int count;      /* Quantity available */
    float price;    /* Price per item */
} Item;

/* Structure to represent the entire inventory */
typedef struct {
    Item items[MAX_ITEMS]; /* Array of items */
    int num_items;         /* Number of items currently in inventory */
} Inventory;

/* Semaphore functions for synchronization */

/* Locks the shared memory to prevent simultaneous access */
void lock(int semid) {
    struct sembuf sb = {0, -1, 0}; /* Decrement semaphore value (lock) */
    semop(semid, &sb, 1);
}

/* Unlocks the shared memory */
void unlock(int semid) {
    struct sembuf sb = {0, 1, 0}; /* Increment semaphore value (unlock) */
    semop(semid, &sb, 1);
}

/* ✅ Function to save the inventory to a file (persistent storage) */
void save_inventory_to_file(int semid, Inventory *inventory) {
    FILE *file = fopen("inventory.txt", "w"); /* Open inventory file in write mode */
    if (!file) {
        perror("Error opening inventory file for writing");
        return;
    }

    lock(semid); /* Lock shared memory before writing */

    /* Write each inventory item to the file */
    for (int i = 0; i < inventory->num_items; i++) {
        fprintf(file, "%s %d %.2f\n",
                inventory->items[i].name, inventory->items[i].count, inventory->items[i].price);
    }
    
    fclose(file);
    unlock(semid); /* Unlock shared memory after writing */

    printf("💾 Inventory saved to file.\n");
}

int main() {
    /* Attach to shared memory */
    int shmid = shmget(SHM_KEY, sizeof(Inventory), 0666);
    if (shmid == -1) {
        perror("Shared memory error");
        exit(1);
    }

    int semid = semget(SEM_KEY, 1, 0666);
    if (semid == -1) {
        perror("Semaphore error");
        exit(1);
    }

    /* Attach shared memory to inventory structure */
    Inventory *inventory = (Inventory *)shmat(shmid, NULL, 0);
    if (inventory == (void *)-1) {
        perror("Shared memory attach error");
        exit(1);
    }

    /* Create FIFO for real-time updates (if it doesn't exist) */
    mkfifo(FIFO_UPDATE, 0666);

    while (1) {
        char item_name[20];
        int new_count;
        float new_price;

        /* Display current inventory */
        printf("\n--- Current Inventory ---\n");
        lock(semid);
        for (int i = 0; i < inventory->num_items; i++) {
            printf("%s - %d items - $%.2f each\n",
                   inventory->items[i].name, inventory->items[i].count, inventory->items[i].price);
        }
        unlock(semid);

        /* Get admin input for updating inventory */
        printf("\nEnter item name to update/add (or type 'exit' to quit): ");
        scanf("%s", item_name);
        if (strcmp(item_name, "exit") == 0) break; /* Exit if admin types "exit" */

        printf("Enter new stock count: ");
        scanf("%d", &new_count);
        printf("Enter new price: ");
        scanf("%f", &new_price);

        /* Update or Add Inventory */
        lock(semid);
        int found = 0;
        for (int i = 0; i < inventory->num_items; i++) {
            if (strcmp(inventory->items[i].name, item_name) == 0) {
                found = 1;

                /* ✅ If stock = 0 & price = 0, remove item */
                if (new_count == 0 && new_price == 0.00) {
                    printf("🗑️ Removing item: %s (Stock = 0 & Price = 0)\n", item_name);

                    /* Shift items left to remove the item */
                    for (int j = i; j < inventory->num_items - 1; j++) {
                        inventory->items[j] = inventory->items[j + 1];
                    }
                    inventory->num_items--;  /* Reduce item count */
                } else {
                    /* Update existing item's stock and price */
                    inventory->items[i].count = new_count;
                    inventory->items[i].price = new_price;
                    printf("✅ Inventory updated: %s - %d items - $%.2f each\n", item_name, new_count, new_price);
                }
                break;
            }
        }

        /* If item is not found, add a new item */
        if (!found) {
            if (inventory->num_items < MAX_ITEMS) {
                strcpy(inventory->items[inventory->num_items].name, item_name);
                inventory->items[inventory->num_items].count = new_count;
                inventory->items[inventory->num_items].price = new_price;
                inventory->num_items++;
                printf("✅ New item added: %s - %d items - $%.2f each\n", item_name, new_count, new_price);
            } else {
                printf("❌ Inventory full! Cannot add new items.\n");
            }
        }
        unlock(semid);

        /* ✅ Save inventory to file after any update */
        save_inventory_to_file(semid, inventory);

        /* Notify shop and customers about stock updates */
        char update_msg[256];
        if (new_count == 0 && new_price == 0.00) {
            sprintf(update_msg, "❌ Item removed: %s", item_name);
        } else {
            sprintf(update_msg, "UPDATE: %s - %d items - $%.2f each", item_name, new_count, new_price);
        }
        
        int fifo_fd = open(FIFO_UPDATE, O_WRONLY);
        if (fifo_fd != -1) {
            write(fifo_fd, update_msg, strlen(update_msg) + 1);
            close(fifo_fd);
        }
    }

    /* Detach from shared memory */
    shmdt(inventory);

    return 0;
}
