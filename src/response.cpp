#include "../inc/webserver.hpp"
#include "../inc/response.hpp"


//!Constructors
Response::Response(Request *req, int clientFd, bool def_error) : _request(req), _clientFd(clientFd), _ready(false), _default_error(def_error)
{
	_response_text = "";
	_html_file = this->_request->getFileName();
	Logger::log("Constructor response call", INFO);
	// for (const auto& pair : _request->getContent()) {
	// 	std::cout << pair.first << ": " << pair.second << std::endl;
	// }
}

Response::~Response() {}

std::string Response::makeStrResponse(void)
{
	std::ostringstream oss;
	std::string httpStatus = _request->getMethod(2);
	std::string code = std::to_string(_request->getErrorCode());
	oss << httpStatus << " " << code << " OK\r\n\r\n";
	oss << _response_text;

	return oss.str();
}


//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::addToBuffer(std::string buffer)
{
	_response_text += buffer;
}

void Response::rSend( void )
{
	// if (_response_text.empty())
	// 	std::cout << "Hey riends" << std::endl;
	// Logger::log(_response_text, WARNING);
	std::string response = _response_text;
	if (!_default_error)
		response = makeStrResponse();
	std::cout << "_______________________________________________\n";
	std::cout << "Message to send =>\n" << response << std::endl;
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