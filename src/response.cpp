#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

#define BUFFER_SIZE 1024

//!Constructors
Response::Response(request req, int clientFd) : _request(req), _fd(clientFd)
{
	html_file = this->_request.get_html();
	std::cout << "Default constructor Response" << std::endl;
	for (const auto& pair : _request.getContent()) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
}

Response::Response(int cgiFd) : _fd(cgiFd), _request(NULL) {}

Response::~Response() {}

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::read_contents()
{
	response_text = read_html_file(html_file);
}

void Response::r_send(){
	//std::cout << "Message to send : " << response_text << std::endl;
	if (send(_fd, response_text.c_str(), response_text.length(), 0) == -1) {
		std::cerr << "Error sending response" << std::endl;
	}
}
