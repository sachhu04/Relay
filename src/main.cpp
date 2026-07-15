#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // 1. Create a socket (IPv4, TCP)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // 2. Set socket options (SO_REUSEADDR allows us to restart the server quickly)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Failed to set setsockopt\n";
        return 1;
    }

    // 3. Bind the socket to port 8080 on all available network interfaces
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(8080);       // Host TO Network Short (endianness conversion)

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    // 4. Listen for incoming connections (backlog of 10)
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Relay listening on port 8080...\n";

    // 5. Accept loop
    while (true) {
        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);
        
        // This call BLOCKS until a client connects
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        std::cout << "Client connected!\n";

        // 6. Send the simplest valid HTTP response
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nHello World";
        write(client_fd, response, strlen(response));

        // 7. Close the client connection
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
