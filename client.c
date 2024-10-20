#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    int PORT;
    char port_buffer[10];
    struct sockaddr_in serverAddress;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    write(1, "Enter the port number: ", strlen("Enter the port number: "));
    ssize_t bytes_read = read(0, port_buffer, sizeof(port_buffer));
    if (bytes_read < 0) {
        perror("Error reading input");
        exit(1);
    }

    port_buffer[bytes_read - 1] = '\0'; // Remove the newline character
    PORT = atoi(port_buffer);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        exit(1);
    }
    printf("You are Now Connected to the server\n");
    while (1) {
        // Receive data from the server
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Error receiving data");
            break;
        }
        if (strcmp(buffer, " Invalid choice") == 0)
            break;
        // Process and display received data
        printf(">%s", buffer);

        // Send a response (optional if no input is expected by client e.g in case of view course)
        char message[1024];
        printf("\n>");
        memset(message, 0, sizeof(message));
        fgets(message, sizeof(message), stdin);

        // Clean the input and response buffers
        message[strlen(message) - 1] = '\0';  // Remove the newline character

        // Send the message to the server
        send(clientSocket, message, strlen(message), 0);

        // Clean the input and response buffers
        memset(buffer, 0, sizeof(buffer));
        memset(message, 0, sizeof(message));
    }
    // Close the socket
    close(clientSocket);

    return 0;
}

