#include "../inc/webserver.hpp"

int c = 0;

void handle_request(int client_fd) {
    // Read the request from the client
    char buffer[1024];
    std::string response;
    int valread = read(client_fd, buffer, sizeof(buffer));
    if (valread < 0) {
        std::cerr << "Error reading request" << std::endl;
        return;
    }

    // Print the received request
    //std::cout << "Received request:\n" << buffer << std::endl;

    // Check if the request contains the button click message
    if (strstr(buffer, "GET /button_clicked") != nullptr) {
        // Handle button click
        std::cout << "Button clicked!" << std::endl;
        ++c;
        std::cout << c << std::endl;
        // Add your action here
    }
    response = read_html_file("index.html");
    // Send the HTTP response back to the client
    if (send(client_fd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error sending response" << std::endl;
    }
}

int main() {
    int c = 0;
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
	std::cout << "Random port generated : " << p << std::endl;
    server_addr.sin_port = htons(p); // Port 8080

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

    std::cout << "Server listening on port " << p << std::endl;

    while (true) {
        // Accept a new connection
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // Print a message when a client connects
        std::cout << "Client connected" << std::endl;

        // Handle the request from the client
        handle_request(client_fd);

        // Close the client socket
        close(client_fd);
    }

    // Close the server socket
    close(server_fd);

    return 0;
}
