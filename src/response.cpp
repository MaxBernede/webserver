#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

#define BUFFER_SIZE 1024

//!Constructors
Response::Response(Request req, int clientFd) : _request(req), _clientFd(clientFd)
{
	_html_file = this->_request.getFileName();
	std::cout << "Default constructor Response" << std::endl;
	for (const auto& pair : _request.getContent()) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
}

Response::~Response() {}

//Try to read an html file and return it as a string with a specific text for response
//if the file doesnt end with .html or is not found/open, this func will call 
//itself again with error.html
std::string Response::makeResponse(std::ifstream &file)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\n\r\n";
	oss << file.rdbuf();
	file.close();

	return oss.str();
}

// TODO: 'html/' should be replaced with the root defined in the config file
std::string Response::readHtmlFile(void)
{
	std::string file_path = "html/" + _html_file;
	std::ifstream file(file_path);

	if (!file.is_open()){
		std::cout << "Error: Impossible to open the file " << file_path << std::endl;
		return "";
	}
	return makeResponse(file);
}

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::readContents(void)
{
	if (!_request.isCgi()) // read from static HTML
	{
		response_text = readHtmlFile();
	}
	else // if it is CGI() read from pipe
	{
		char buffer[BUFFER_SIZE];
		int ret = read(_readFd, buffer, BUFFER_SIZE);
		if (ret < 0)
			throw(Exception("Reading static file failed", errno));
		if (ret == 0)
		{
			//update the status of the pollFd
		}
	}
}

void Response::rSend(){
	//std::cout << "Message to send : " << response_text << std::endl;
	if (send(_clientFd, response_text.c_str(), response_text.length(), 0) == -1) {
		std::cerr << "Error sending response" << std::endl;
	}
}
