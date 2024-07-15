#include "../inc/webserver.hpp"
#include "../inc/response.hpp"


//!Constructors
Response::Response(Request *req, int clientFd, bool def_error) : _request(req), _clientFd(clientFd), _ready(false), _default_error(def_error)
{
	_html_file = this->_request->getFileName();
	Logger::log("Constructor response call", INFO);
	for (const auto& pair : _request->getContent()) {
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
	oss << "HTTP/1.1 200 OK\r\n\r\n";
	oss << file.rdbuf();
	file.close();

	return oss.str();
}

std::string Response::makeStrResponse(void)
{
	std::ostringstream oss;
	std::string httpStatus = _request->getMethod(2);
	oss << httpStatus << " 200 OK\r\n\r\n";
	oss << _response_text;

	return oss.str();
}

// TODO: 'html/' should be replaced with the root defined in the config file
std::string Response::readHtmlFile(void)
{
	std::string file_path = "html/" + _html_file;
	std::ifstream _file(file_path);

	if (!_file.is_open()){
		std::cout << "Error: Impossible to open the file " << file_path << std::endl;
		return "";
	}
	return makeResponse(_file);
}

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::addToBuffer(std::string buffer)
{
	_response_text += buffer;
}

void Response::rSend( void )
{
	std::string response = _response_text;
	if (!_default_error)
		response = makeStrResponse();
	std::cout << "_______________________________________________\n";
	std::cout << "Message to send =>\n " << response << std::endl;
	std::cout << "_______________________________________________\n";
	if (send(_clientFd, response.c_str(), response.length(), 0) == -1)
	{
		std::cout << "ERROR with SEND " << _clientFd << std::endl;
		throw(Exception("Error sending response", errno));
	}
}

void Response::setReady( void )
{
	_ready = true;
}

void Response::setResponseString(std::string response)
{
	_response_text = response;
}