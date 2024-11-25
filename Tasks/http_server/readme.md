#  🌐 HTTP Server in C

This project is a simple HTTP server implemented in C, designed to handle basic HTTP requests and serve files or directories. It includes functionality for:

- Serving static files.
- Listing directory contents.
- Executing CGI scripts.

---

## 🌟 Features

### Core Functionalities
- **Handles HTTP GET requests**: The server processes only GET requests and responds with appropriate content.
- **Serves files**: Returns the requested file if it exists.
- **Directory listing**: Displays the contents of a directory if the requested path is a folder.
- **CGI script execution**: Executes `.cgi` scripts and returns their output.

### Limitations
- Only supports HTTP/1.1.
- Only handles GET requests; other HTTP methods return a `405 Method Not Allowed` response.

---

## 🗂️ Prerequisites

### System Requirements
- A POSIX-compliant operating system.
- GCC compiler for building the server.

### Libraries Used
- `<stdio.h>`
- `<stdlib.h>`
- `<string.h>`
- `<unistd.h>`
- `<arpa/inet.h>`
- `<sys/stat.h>`
- `<sys/types.h>`
- `<dirent.h>`
- `<fcntl.h>`
- `<errno.h>`

---

## 🚀  How to Build and Run

### Compilation
To compile the server, use the GCC compiler:
```bash
gcc -o http_server http_server.c
```

### Running the Server
Execute the server with:
```bash
./http_server
```
By default, the server listens on port 8080.

## ⚙️ Code Breakdown
### Key Components
1. #### `main` Function:
    - Creates and configures a server socket.
    - Listens for incoming connections and forks a process for each client.

2. #### Request Handling:

    - `handle_request`: Parses HTTP requests and determines appropriate actions.
    - Routes the request to:
        - `list_directory`: For directory paths.
        - `send_file`: For regular files.
        - `execute_cgi`: For .cgi files.

2. ##### Response Handling:
    - `send_response`: Sends HTTP responses to the client.
    - Dynamic construction of headers and bodies for responses.


## 📄 Example Usage
1. #### Serve a File:
    - Place a file (e.g., `index.html`) in the server's directory.
    - Access it via `http://<server-ip>:8080/index.html`.

2. #### List a Directory:
    - Ensure a directory (e.g., `files/`) exists in the server's path.
    - Access it via `http://<server-ip>:8080/files`.

3. #### Run a CGI Script:
    - Place an executable script (e.g., script.cgi) in the server's directory.
    - Access it via `http://<server-ip>:8080/script.cgi`.

4. #### See the Example Video in the link below:
    - [View Video](https://drive.google.com/file/d/1FO4wPzamExZu_Ow3K95r-n_mJnXJorzw/view?usp=sharing)

## ⚠️ Error Handling
- 404 Not Found: If the requested file or directory does not exist.
- 405 Method Not Allowed: For unsupported HTTP methods.
- 500 Internal Server Error: For server-side issues, such as file access or CGI execution failures.

## 🐞 Known Issues
- Limited to serving small files due to the buffer size (BUF_SIZE = 1024).
- Basic HTTP parsing; complex or malformed requests may not be handled gracefully.
- Lack of SSL/TLS support.

## ✨ Future Enhancements
- Add support for more HTTP methods (e.g., POST).
- Implement chunked transfer encoding for large files.
- Integrate SSL/TLS for secure connections.

## 🙏 Acknowledgments
This project was developed as a demonstration of basic HTTP server functionality in C. Contributions and improvements are welcome! 🚀


