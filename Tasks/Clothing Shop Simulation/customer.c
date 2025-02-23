#include <stdio.h>       /* Standard I/O operations */
#include <stdlib.h>      /* Standard library for memory management, process control, etc. */
#include <string.h>      /* String manipulation functions */
#include <fcntl.h>       /* File control operations (used for FIFO pipes) */
#include <sys/stat.h>    /* File permissions and status functions */
#include <unistd.h>      /* System calls like open(), read(), write(), and close() */

#define FIFO_REQUEST "/tmp/shop_fifo"       /* FIFO path for sending requests to the shop */
#define FIFO_RESPONSE "/tmp/inventory_fifo" /* FIFO path for receiving inventory data */
#define FIFO_CONFIRM "/tmp/confirm_fifo"    /* FIFO path for receiving order confirmations */

int main() {
    char buffer[1024]; /* Buffer to store messages received from FIFO */

    while (1) {  
        /* Loop to allow the customer to place multiple orders until they choose to exit */

        /* Open the FIFO for writing an inventory request */
        int fifo_fd = open(FIFO_REQUEST, O_WRONLY);
        if (fifo_fd == -1) {
            perror("Error opening shop FIFO"); /* Print error if FIFO cannot be opened */
            exit(1); /* Exit the program with an error code */
        }

        /* Send an inventory request to the shop */
        write(fifo_fd, "inventory_request", strlen("inventory_request") + 1);
        close(fifo_fd); /* Close the FIFO after writing */

        /* Open the FIFO to read the inventory response from the shop */
        fifo_fd = open(FIFO_RESPONSE, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Error opening inventory FIFO for reading"); /* Print error if FIFO cannot be opened */
            exit(1); /* Exit the program */
        }

        /* Read the inventory data from the FIFO */
        int bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);
        close(fifo_fd); /* Close FIFO after reading */
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';  /* Null-terminate the string to prevent garbage output */
            printf("\n--- Available Inventory ---\n%s\n", buffer);
        } else {
            printf("❌ Error: No inventory data received from shop.\n");
            continue; /* Restart the loop if no inventory data is received */
        }

        /* Prompt the user to enter an item name or exit */
        char item_name[20];
        int quantity;

        printf("Enter item name (or type 'exit' to quit): ");
        scanf("%s", item_name);

        /* If the user enters "exit", terminate the process */
        if (strcmp(item_name, "exit") == 0) {
            printf("👋 Exiting customer process...\n");
            break;
        }

        /* Ask the user to enter the quantity they want to purchase */
        printf("Enter quantity: ");
        scanf("%d", &quantity);

        /* Format the order message as "item_name quantity" */
        char order_msg[256];
        sprintf(order_msg, "%s %d", item_name, quantity);

        /* Open the FIFO for writing the order request */
        fifo_fd = open(FIFO_REQUEST, O_WRONLY);
        write(fifo_fd, order_msg, strlen(order_msg) + 1);
        close(fifo_fd); /* Close FIFO after writing the order */

        /* Open the FIFO to read the order confirmation from the shop */
        fifo_fd = open(FIFO_CONFIRM, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Error opening confirm FIFO"); /* Print error if FIFO cannot be opened */
            exit(1); /* Exit the program */
        }

        /* Read the confirmation message from the shop */
        bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);
        close(fifo_fd); /* Close FIFO after reading */
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';  /* Null-terminate the string */
            printf("%s\n", buffer); /* Print the confirmation message */
        } else {
            printf("❌ Error: No confirmation received from shop.\n");
        }
    }

    return 0; /* End of program */
}
