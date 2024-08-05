#include "Response.hpp"


Response::Response(int clientFd) : _clientFd(clientFd), _ready(false)
{
	_response_text = "";
}

Response::~Response() {}

//! check the fck is going on with the 204 No response 2 headers?
void Response::addHeaders(Request *request)
{
	std::ostringstream oss;
	std::string http= request->getMethod(2);
	std::string code = std::to_string(request->getErrorCode());
	std::string message = httpStatus[request->getErrorCode()]; // TODO There is no message for directory listing
	oss << http << " " << code << " " << message ;
	oss << "\nConnection: close";
	oss << "\r\n\r\n";
	oss << _response_text;

	_response_text = oss.str();
}

void Response::errorResponseHTML(ErrorCode error) {
	std::string text = httpStatus[int(error)];
	_response_text = START_HTML + std::to_string(error) + ", " + text + END_HTML;
}

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::addToBuffer(std::string buffer)
{
	_response_text += buffer;
}

void Response::rSend()
{
	Logger::log("Sending Response to client...");
	Logger::log("TEXT:\n" + _response_text, LogLevel::DEBUG);
	if (send(_clientFd, _response_text.c_str(), _response_text.length(), 0) == -1)
		Logger::log("Error with sending response", LogLevel::ERROR);
}

void Response::sendRedir()
{
	if (send(_clientFd, _response_text.c_str(), _response_text.length(), 0) == -1)
		Logger::log("Error with send!", LogLevel::ERROR);
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
	std::string to = "";
	// std::string val = "";
	for (s_redirect r : redirs){
		if (r.redirFrom == from){
			to = r.redirTo;
			// val = std::to_string(r.returnValue);
			break ;
		}
	}
	if (to == "")
	{
		return (INTERNAL_SRV_ERR);
	}
	std::ostringstream oss;
	oss << "HTTP/1.1 ";
	// oss << val;
	oss << status;
	oss << " Redirection\r\n";
	oss << "Content-Type: text; charset=utf-8\r\n";
	oss << "Location: ";
	oss << to << "\r\n\r\n";
	oss << REDIR_START << "<a href=\"" << to << "\">" << to << "</a>" << REDIR_END;
	std::cout << oss.str() << std::endl;
	_response_text = oss.str();
	return (status);
}

void Response::setDirectoryListing(Request *request)
{
	std::string	name = request->getConfig().getBasePath() + request->getConfig().getRoot() + request->getFileName();
	if (!exists(name))
		throw (HTTPError(PAGE_NOT_FOUND));
	std::vector<std::string>	v = getDirectoryContent(name.c_str());
	std::ostringstream oss;
	 addHeaders(request);
	oss << _response_text;
	oss << DIR_LIST_START;
	oss << name;
	oss << DIR_LIST_MID;
	for (std::string s : v)
	{
		std::string link = name + s;
		if (isDirectory(link.c_str())){
			s += '/';
		}
		oss << "<li><a href=\"" << s << "\">";
		oss << s << "</a></li>";
	}
	oss << DIR_LIST_END;
	_response_text = oss.str();
}
