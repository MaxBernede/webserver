#include "../inc/webserver.hpp"
#include "../inc/response.hpp"


//!Constructors
Response::Response(Request *req, int clientFd) : _request(req), _clientFd(clientFd), ready(false)
{
	_html_file = this->_request->getFileName();
	// std::cout << "Default constructor Response" << std::endl;
	// for (const auto& pair : _request->getContent()) {
	// 	std::cout << pair.first << ": " << pair.second << std::endl;
	// }
}

Response::~Response() {}

//Try to read an html file and return it as a string with a specific text for response
//if the file doesnt end with .html or is not found/open, this func will call 
//itself again with error.html
std::string Response::makeResponse(std::ifstream &file){
	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	oss << file.rdbuf();
	file.close();

	return oss.str();
}

std::string Response::makeStrResponse(void){
	std::ostringstream oss;
	std::string httpStatus = _request->getMethod(2);

	//Below is to get the content type
	std::string file = _request->getFileNameProtected();
	oss << httpStatus << " ";
	oss << "200 OK\r\n";
	oss << "Content-Type: " << contentType.at(getExtension(file)) << "\r\n";
	oss << "\r\n";
	oss << response_text;

	return oss.str();
}

// don't know if this works yet, have to figure out how to test this properly
std::string Response::redirectResponse(void){
	std::string from = _request->getFileNameProtected();
	std::string to;
	std::string val;
	std::list<s_redirect> redirs = (_request->getConfig()).getRedirect();
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
	oss << " Permanent Redirect \r\nLocation: ";
	oss << to;
	if (send(_clientFd, oss.str().c_str(), oss.str().length(), 0) == -1)
	{
		throw(Exception("Error sending response", errno));
	}
	std::cout << oss.str() << std::endl;
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
void Response::addToBuffer(std::string buffer)
{
	response_text += buffer;
}

// TODO: in stead of sending everything at once, responses should also be chunked,
// just like requests are, I think
void Response::rSend( void ){
	std::string response = makeStrResponse();
	// std::cout << "_______________________________________________\n";
	// std::cout << "Message to send =>\n " << response << std::endl;
	// std::cout << "_______________________________________________\n";
	if (send(_clientFd, response.c_str(), response.length(), 0) == -1)
	{
		throw(Exception("Error sending response", errno));
	}
}

void Response::setReady( void )
{
	ready = true;
}
