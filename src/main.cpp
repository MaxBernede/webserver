#include "../inc/webserver.hpp"

int main() {
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Prepare the server address structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
	int p = genererPort();
    std::cout << "http://localhost:" << p << std::endl;
    server_addr.sin_port = htons(p);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    while (true) {
        // Accept a new connection
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        Response response(client_fd);
        std::cout << "Client connected" << std::endl;
        // Handle the request from the client
        response.handle_request();

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
