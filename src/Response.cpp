#include "Response.hpp"


Response::Response(int clientFd) : _clientFd(clientFd), _ready(false)
{
	_response_text = "";
}

Response::~Response() {}

//! check the fck is going on with the 204 No response 2 headers?
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

void Response::errorResponseHTML(ErrorCode error) {
	std::string text = httpStatus[int(error)];
	_response_text = START_HTML + std::to_string(error) + ", " + text + END_HTML;
	Logger::log(_response_text, ERROR);
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
		// throw(Exception("Error sending response", errno));
	}
}

void Response::sendRedir()
{
	if (send(_clientFd, _response_text.c_str(), _response_text.length(), 0) == -1)
		Logger::log("Error with send!", LogLevel::ERROR);
	Logger::log("Redirection sent successfully", LogLevel::INFO);
}

void Response::setReady( void )
{
	_ready = true;
}

void Response::setResponseString(std::string response)
{
	_response_text = response;
}

int Response::setRedirectStr(int status, std::string from, std::list<s_redirect> redirs)
{
	std::string to;
	std::string val;
	for (s_redirect r : redirs){
		if (r.redirFrom == from){
			to = r.redirTo;
			val = std::to_string(r.returnValue);
			break ;
		}
	}
	if (to == "")
	{
		// Internal Server Error
		return (500);
	}
	// std::cout << "do a thing" << std::endl << std::endl;
	std::ostringstream oss;
	oss << "HTTP/1.1 ";
	// oss << val;
	oss << status;
	oss << " Redirection\r\n";
	oss << "Content-Type: text; charset=utf-8\r\n";
	oss << "Location: ";
	oss << to << "\r\n\r\n";
	oss << REDIR_START << to << REDIR_END;
	std::cout << oss.str() << std::endl;
	_response_text = oss.str();
	return (status);
}
