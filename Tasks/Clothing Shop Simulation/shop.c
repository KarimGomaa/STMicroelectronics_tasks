#include <stdio.h>       /* Standard I/O operations */
#include <stdlib.h>      /* Standard library for memory management, process control, etc. */
#include <string.h>      /* String manipulation functions */
#include <fcntl.h>       /* File control operations (used for FIFO pipes) */
#include <sys/stat.h>    /* File permissions and status functions */
#include <unistd.h>      /* System calls like open(), read(), write(), and close() */
#include <sys/shm.h>     /* Shared memory operations */
#include <sys/sem.h>     /* Semaphore operations */
#include <time.h>        /* Time functions for logging */

#define FIFO_REQUEST "/tmp/shop_fifo"       /* FIFO path for receiving customer orders */
#define FIFO_RESPONSE "/tmp/inventory_fifo" /* FIFO path for sending inventory data to customers */
#define FIFO_CONFIRM "/tmp/confirm_fifo"    /* FIFO path for sending order confirmations */
#define FIFO_UPDATE "/tmp/inventory_update_fifo" /* FIFO path for real-time inventory updates */

#define SHM_KEY 5678  /* Shared memory key */
#define SEM_KEY 91011 /* Semaphore key */
#define MAX_ITEMS 15  /* Maximum number of inventory items */

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

int shmid, semid;    /* Shared memory and semaphore IDs */
Inventory *inventory; /* Pointer to shared inventory */

/* Semaphore functions for synchronization */

/* Locks the shared memory to prevent simultaneous access */
void lock() {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

/* Unlocks the shared memory */
void unlock() {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

/* Function to listen for real-time inventory updates from the inventory manager */
void listen_for_updates() {
    char buffer[256];

    /* Create the FIFO if it doesn't exist */
    mkfifo(FIFO_UPDATE, 0666);

    while (1) {
        int fifo_fd = open(FIFO_UPDATE, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Error opening inventory update FIFO");
            continue;
        }

        /* Read update message from FIFO */
        read(fifo_fd, buffer, sizeof(buffer));
        close(fifo_fd);

        if (strlen(buffer) > 0) {
            printf("----------------------------------------------------------------\n");
            printf("🔄 Real-time Update: %s\n", buffer);
        }
    }
}

/* Function to load inventory from a file into shared memory */
void load_inventory() {
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        perror("Error opening inventory file");
        exit(1);
    }

    lock(); /* Lock shared memory before updating inventory */
    inventory->num_items = 0;

    /* Read inventory data from file */
    while (fscanf(file, "%s %d %f", 
                  inventory->items[inventory->num_items].name, 
                  &inventory->items[inventory->num_items].count, 
                  &inventory->items[inventory->num_items].price) != EOF) {
        inventory->num_items++;
    }
    
    fclose(file);
    unlock(); /* Unlock shared memory after updating */

    /* Print loaded inventory for debugging */
    printf("\n✅ Inventory Loaded from File:\n");
    for (int i = 0; i < inventory->num_items; i++) {
        printf("%s - %d items - $%.2f each\n",
               inventory->items[i].name, inventory->items[i].count, inventory->items[i].price);
    }
}

/* Function to send current inventory to customers */
void send_inventory() {
    char buffer[1024] = "";
    
    lock(); /* Lock shared memory before reading inventory */
    for (int i = 0; i < inventory->num_items; i++) {
        char item_details[100];
        sprintf(item_details, "%s - %d items - $%.2f each\n",
                inventory->items[i].name, inventory->items[i].count, inventory->items[i].price);
        strcat(buffer, item_details);
    }
    unlock(); /* Unlock shared memory */

    /* Open FIFO and write inventory data */
    int fifo_fd = open(FIFO_RESPONSE, O_WRONLY);
    if (fifo_fd == -1) {
        perror("Error opening inventory FIFO for writing");
        return;
    }
    write(fifo_fd, buffer, strlen(buffer) + 1);
    close(fifo_fd);

    printf("✅ Inventory sent to customer.\n");
}

/* Function to log customer orders in "orders.log" */
void log_order(const char *item_name, int quantity) {
    FILE *file = fopen("orders.log", "a+"); /* Open file in append mode */
    if (file == NULL) {
        perror("Error opening order log file");
        return;
    }

    /* Get current timestamp */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    /* Write order details to file */
    fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] Order: %d x %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            quantity, item_name);

    fclose(file);
}

/* Function to process customer orders */
void process_orders() {
    char buffer[256];

    while (1) {
        int fifo_fd = open(FIFO_REQUEST, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Error opening shop FIFO");
            continue;
        }
        read(fifo_fd, buffer, sizeof(buffer));
        close(fifo_fd);

        if (strcmp(buffer, "inventory_request") == 0) {
            send_inventory();
            continue;
        }

        char item_name[20];
        int quantity;
        sscanf(buffer, "%s %d", item_name, &quantity);

        char confirm_msg[256];

        lock();
        int found = 0;
        for (int i = 0; i < inventory->num_items; i++) {
            if (strcmp(inventory->items[i].name, item_name) == 0) {
                found = 1;
                if (inventory->items[i].count >= quantity) {
                    inventory->items[i].count -= quantity;

                    printf("----------------------------------------------------------\n");
                    printf("✅ Order fulfilled: %d x %s\n", quantity, item_name);

                    /* Send order confirmation */
                    sprintf(confirm_msg, "✅ Order fulfilled: %d x %s", quantity, item_name);

                    /* Log the order */
                    log_order(item_name, quantity);
                } else {
                    printf("----------------------------------------------------------\n");
                    sprintf(confirm_msg, "❌ Error: Not enough stock for %s (Available: %d)", 
                            item_name, inventory->items[i].count);
                }
                break;
            }
        }
        unlock();

        if (!found) {
            sprintf(confirm_msg, "❌ Error: Item '%s' not found!", item_name);
        }

        /* Send confirmation message to customer */
        fifo_fd = open(FIFO_CONFIRM, O_WRONLY);
        write(fifo_fd, confirm_msg, strlen(confirm_msg) + 1);
        close(fifo_fd);
    }
}

int main() {
    /* Setup shared memory & semaphore */
    shmid = shmget(SHM_KEY, sizeof(Inventory), 0666 | IPC_CREAT);
    semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    inventory = (Inventory *)shmat(shmid, NULL, 0);
    semctl(semid, 0, SETVAL, 1);

    /* Ensure all FIFOs are created */
    mkfifo(FIFO_REQUEST, 0666);
    mkfifo(FIFO_RESPONSE, 0666);
    mkfifo(FIFO_CONFIRM, 0666);

    /* Start listening for inventory updates in a separate process */
    if (fork() == 0) {
        listen_for_updates();
        exit(0);
    }

    load_inventory();  /* Load inventory from file */
    process_orders();  /* Start processing customer orders */

    return 0;
}
