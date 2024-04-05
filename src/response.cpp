#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

//if get found, skip it and search the next word. 
//if word is '/' change it for index.html
//if problem with words, return error.html
std::string parse_buffer(std::string buffer){
	std::string keyword = "GET";

	size_t pos = buffer.find(keyword);
    if (pos != std::string::npos) {
        size_t next_pos = pos + keyword.length() + 1;

        size_t space_pos = buffer.find(" ", next_pos);

        // Extract the next word
        std::string next_word;
        if (space_pos != std::string::npos)
            next_word = buffer.substr(next_pos, space_pos - next_pos);
        else
            next_word = buffer.substr(next_pos);

        std::cout << "Next word after 'GET': " << next_word << std::endl;
		if (next_word == "/" || endsWith(buffer, ".html"))
		{
			//std::cout << "return index.html" << std::endl;
			return "index.html";
		}
		return next_word;
    } 
	else
        std::cout << "Keyword 'GET' not found" << std::endl;
	return "error.html";
}

void handle_request(int client_fd) {
    // Read the request from the client
    char buffer[1024];
    std::string response;

    int valread = read(client_fd, buffer, sizeof(buffer));
    if (valread < 0) {
        std::cerr << "Error reading request" << std::endl;
        return;
    }

    std::cout << "Received request:\n" << buffer << std::endl;

    // Check if the request contains the button click message
	std::string html_file;

	html_file = parse_buffer(buffer);
    response = read_html_file(html_file);
    // Send the HTTP response back to the client
    if (send(client_fd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error sending response" << std::endl;
    }
}