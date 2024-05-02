#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

#define BUFFER_SIZE 1024

//!Constructors
Response::Response(request req, int clientFd) : _request(req), _writeFd(clientFd), _readFd(-1)
{
	html_file = this->_request.get_html();
	std::cout << "Default constructor Response" << std::endl;
	for (const auto& pair : _request.getContent()) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
}

Response::Response(int cgiFd, int clientFd) : _request(NULL), _writeFd(clientFd), _readFd(cgiFd) {}

Response::~Response() {}

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::read_contents()
{
	if (!_request.isCgi())
	{
		response_text = read_html_file(html_file);
	}
	else
	{
		// read from the pipe
		char buffer[1000];
		int ret = read(_readFd, buffer, 1000);


	}
}

void Response::r_send(){
	//std::cout << "Message to send : " << response_text << std::endl;
	if (send(_writeFd, response_text.c_str(), response_text.length(), 0) == -1) {
		std::cerr << "Error sending response" << std::endl;
	}
}
