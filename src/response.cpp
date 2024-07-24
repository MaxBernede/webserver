#include "../inc/webserver.hpp"
#include "../inc/response.hpp"
#include "HTTPStatus.hpp"
#include "Redirect.hpp"

//!Constructors

Response::Response(int clientFd) : _clientFd(clientFd), _ready(false)
{
	_response_text = "";
}

Response::~Response() {}

std::string Response::makeStrResponse(Request *request)
{
	std::ostringstream oss;
	std::string http= request->getMethod(2);
	std::string code = std::to_string(request->getErrorCode());
	std::string message = httpStatus[request->getErrorCode()];
	oss << http << " " << code << " " << message;
	oss << "\r\n\r\n";
	oss << _response_text;

	return oss.str();
}


//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::addToBuffer(std::string buffer)
{
	_response_text += buffer;
}

void Response::rSend( Request *request )
{
	std::string response = _response_text;
	response = makeStrResponse(request);
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

// somehow _request points to zero page, trying to fix this
std::string Response::redirectResponse(int clientFd, std::string from, std::list<s_redirect> redirs)
{
	// std::string from = _request->getFileNameProtected();
	std::string to;
	std::string val;
	// std::list<s_redirect> redirs = (_request->getConfig()).getRedirect();
	for (s_redirect r : redirs){
		if (r.redirFrom == from){
			to = r.redirTo;
			val = std::to_string(r.returnValue);
			break ;
		}
	}
	if (to == "")
		throw Exception("unexpected redirect error", 300);
	std::cout << "do a thing" << std::endl << std::endl;
	std::ostringstream oss;
	oss << "HTTP/1.1 ";
	oss << val;
	oss << " Redirection\r\n";
	oss << "Content-Type: text; charset=utf-8\r\n";
	oss << "Location: ";
	oss << to << "\r\n\r\n";
	oss << REDIR_START << to << REDIR_END;
	if (send(clientFd, oss.str().c_str(), oss.str().length(), 0) == -1)
	{
		throw(Exception("Error sending response", errno));
	}
	std::cout << oss.str() << std::endl;
	return oss.str();
}
