#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "header/bank_employee.h"

// Forward declarations for functions you need to define

void handle_client(int clientSocket) {
    
    int bytes_received;
    int flag = 1;

    while (flag) {
    char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        char sendMessage[] = "\n\n....................................Banking Management System ....................................\n"
                    "Please select your role:\n"
                    "1.) Customer\n"
                    "2.) Bank Employee\n"
                    "3.) Manager\n"
                    "4.) Adminstrator\n"
                    "5.) Exit\n"
                    "Enter the corresponding number to proceed: ";
	write(clientSocket, sendMessage, sizeof(sendMessage));

        bytes_received = read(clientSocket, buffer, sizeof(buffer));

        if (bytes_received <= 0) {
            break;  // Client disconnected or error occurred.
        }
        
        buffer[bytes_received]='\0';
        int retIndex = atoi(buffer);

        switch (retIndex) {
            case 1:
                 	customerHandle(clientSocket);
                break;
            case 2:
                	employeeHandle(clientSocket);
                break;
           
            case 3:
                         managerHandle(clientSocket);
                break;
            case 4:      
                         adminHandle(clientSocket);
                break;
               
            default:
                //printf("4 th option\n");
                char errorMsg[] = " Invalid choice";
                write(clientSocket, errorMsg, sizeof(errorMsg));

                break;
        }
    }

    close(clientSocket);
}


int main() {
        int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    int port;
    char port_buffer[10]; // Buffer for reading the port as a string

    write(1, "Enter the port number: ", strlen("Enter the port number: "));
    ssize_t bytes_read = read(0, port_buffer, sizeof(port_buffer));
    if (bytes_read < 0) {
        perror("Error reading input");
        close(server_socket);
        exit(1);
    }

    port_buffer[bytes_read - 1] = '\0'; // Remove the newline character
    port = atoi(port_buffer);

    // Bind the socket to an address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port); // Use the port entered by the user
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_socket);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Listening failed");
        close(server_socket);
        exit(1);
    }

    printf("Server is listening on port %d...\n", port);

    while (1) {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        int pid = fork();
        if (pid == 0) {
            // Child process
            //printf("In child process\n");
            close(server_socket);  // Close the parent's socket
            handle_client(client_socket);
            exit(0);
        } else if (pid < 0) {
            perror("Fork failed");
            close(client_socket);
        } else {
            // Parent process
            close(client_socket);  // Close the child's socket
        }
    }

    close(server_socket);
    return 0;
}
