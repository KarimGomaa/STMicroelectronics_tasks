#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080
#define BUF_SIZE 1024
#define BACKLOG 10

void handle_request(int client_fd);
void send_response(int client_fd, const char *status, const char *content_type, const char *body);
void list_directory(int client_fd, const char *path);
void send_file(int client_fd, const char *path);
void execute_cgi(int client_fd, const char *path);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to the specified port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, BACKLOG) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("HTTP server running on port %d\n", PORT);

    // Accept and handle client connections
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) != -1) {
        if (fork() == 0) { // Child process handles the request
            close(server_fd);
            handle_request(client_fd);
            close(client_fd);
            exit(0);
        }
        close(client_fd); // Parent process closes the socket
    }

    perror("Accept failed");
    close(server_fd);
    return 0;
}

void handle_request(int client_fd) {
    char buffer[BUF_SIZE];
    read(client_fd, buffer, BUF_SIZE);

    printf("Request:\n%s\n", buffer);

    // Simple HTTP parsing (assumes GET method)
    char method[16], path[256];
    sscanf(buffer, "%s %s", method, path);

    if (strcmp(method, "GET") != 0) {
        send_response(client_fd, "405 Method Not Allowed", "text/plain", "Only GET supported");
        return;
    }

    // Remove leading slash
    if (path[0] == '/') memmove(path, path + 1, strlen(path));

    struct stat path_stat;
    if (stat(path, &path_stat) == -1) {
        send_response(client_fd, "404 Not Found", "text/plain", "File or directory not found");
    } else if (S_ISDIR(path_stat.st_mode)) {
        list_directory(client_fd, path);
    } else if (S_ISREG(path_stat.st_mode)) {
        if (strstr(path, ".cgi")) {
            execute_cgi(client_fd, path);
        } else {
            send_file(client_fd, path);
        }
    }
}

void send_response(int client_fd, const char *status, const char *content_type, const char *body) {
    char response[BUF_SIZE];
    snprintf(response, BUF_SIZE, "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n%s",
             status, content_type, strlen(body), body);
    write(client_fd, response, strlen(response));
}

void list_directory(int client_fd, const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        send_response(client_fd, "500 Internal Server Error", "text/plain", "Cannot open directory");
        return;
    }

    char body[BUF_SIZE] = "<html><body><ul>";
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        strcat(body, "<li>");
        strcat(body, entry->d_name);
        strcat(body, "</li>");
    }
    strcat(body, "</ul></body></html>");
    closedir(dir);

    send_response(client_fd, "200 OK", "text/html", body);
}

void send_file(int client_fd, const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        send_response(client_fd, "500 Internal Server Error", "text/plain", "Cannot open file");
        return;
    }

    char body[BUF_SIZE];
    size_t bytes_read = fread(body, 1, BUF_SIZE, file);
    fclose(file);

    send_response(client_fd, "200 OK", "text/plain", body);
}

void execute_cgi(int client_fd, const char *path) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        send_response(client_fd, "500 Internal Server Error", "text/plain", "CGI execution failed");
        return;
    }

    if (fork() == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        execl(path, path, NULL);
        exit(1);
    }

    close(pipe_fd[1]);
    char body[BUF_SIZE];
    read(pipe_fd[0], body, BUF_SIZE);
    close(pipe_fd[0]);

    send_response(client_fd, "200 OK", "text/html", body);
}
