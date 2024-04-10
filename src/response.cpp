#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

//if get found, skip it and search the next word. 
//if word is '/' change it for index.html
//if problem with words, return error.html
std::string Response::parse_buffer(std::string buffer){
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

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::handle_request() {
    char buffer[1024];
    std::cout << client_fd;
    if (read(client_fd, buffer, sizeof(buffer)) < 0){
        std::cerr << "Error reading request" << std::endl;
        return;
    }

	html_file = parse_buffer(buffer);
    response_text = read_html_file(html_file);

    r_send();
}

void Response::r_send(){
    std::cout << "Message to send : " << response_text << std::endl;
    if (send(client_fd, response_text.c_str(), response_text.length(), 0) == -1) {
        std::cerr << "Error sending response" << std::endl;
    }
}

//!Constructors
Response::Response(){
    std::cout << "Default constructor Response" << std::endl;
}
Response::Response(int fd){
    std::cout << "Constructor Response" << std::endl;
    client_fd = fd;
}
//!Getters
int Response::get_client_fd(){
    return client_fd;
}