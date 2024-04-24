#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::handle_request() {
    char buffer[1024];
    if (read(client_fd, buffer, sizeof(buffer)) < 0){
        std::cerr << "Error reading request" << std::endl;
        return;
    }
    request request(buffer);
	html_file = request.get_html();
    response_text = read_html_file(html_file);

    r_send();
}

void Response::r_send(){
    //std::cout << "Message to send : " << response_text << std::endl;
    if (send(client_fd, response_text.c_str(), response_text.length(), 0) == -1) {
        std::cerr << "Error sending response" << std::endl;
    }
}



//!Constructors
Response::Response(){
    std::cout << "Default constructor Response" << std::endl;
}
Response::Response(int fd){
    //std::cout << "Constructor Response" << std::endl;
    client_fd = fd;
}



//!Getters
int Response::get_client_fd(){
    return client_fd;
}