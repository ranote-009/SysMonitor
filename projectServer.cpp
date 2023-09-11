#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Bind the socket to an IP address and port
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345); // Use the desired port number
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        return 1;
    }

    std::cout << "Server is listening..." << std::endl;

    // Accept a connection from a client
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);

    if (client_socket == -1) {
        std::cerr << "Error accepting client connection" << std::endl;
        return 1;
    }

    std::cout << "Client connected" << std::endl;

    // Send a "Hello" message to the client
    const char* message = "Hello, client!";
    send(client_socket, message, strlen(message), 0);

    // Receive acknowledgment from the client
    char acknowledgment[1024];
    recv(client_socket, acknowledgment, sizeof(acknowledgment), 0);
    std::cout << "Received acknowledgment from client: " << acknowledgment << std::endl;

    // Close the client and server sockets
    close(client_socket);
    close(server_socket);

    return 0;
}
