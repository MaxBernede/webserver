#include "HTTPObject.hpp"

HTTPObject::HTTPObject(int clientFd) : 
				_clientFd(clientFd),
				_cgi(NULL)
{
	_request = new Request(clientFd);
	_response = new Response(clientFd);
}

HTTPObject::~HTTPObject()
{
	delete _request;
	delete _response;
	if (_cgi != NULL)
	{
		delete _cgi;
	}
}

void	HTTPObject::sendRedirection()
{
	_response->sendRedir();
}

void	HTTPObject::createCgi()
{
	_cgi = new CGI(_request, _clientFd);
}

std::string	formatStrToHTML(std::string str)
{
	std::string	result;
	std::string	htmlFormat;

	std::istringstream	iss(str);

	for (std::string line; std::getline(iss, line); )
	{
		htmlFormat = "<p>" + line + "</p>";
		result += htmlFormat;
	}
	return (result);
}

void	HTTPObject::writeToCgiPipe()
{
	std::string b = _request->getValues("Body");
	std::string body = formatStrToHTML(b);
	if (body.length() > 0) {
		Logger::log("Body: " + body, LogLevel::INFO);
		size_t totalBytesWritten = 0;
		ssize_t bytesWritten;
		while (totalBytesWritten < body.length()) {
			bytesWritten = write(_writeFd, body.c_str() + totalBytesWritten, body.length() - totalBytesWritten);
			if (bytesWritten == -1) {
				throw(Exception("Write failed", 1));
			}
			totalBytesWritten += bytesWritten;
		}
	}
	_doneWritingToCgi = true; // Consider the purpose of this variable if it has no use yet
}


void	HTTPObject::runCgi()
{
	_cgi->run();
}

void	HTTPObject::sendResponse()
{
	_response->rSend(_request);
}

bool	HTTPObject::isCgi()
{
	return (_request->isCgi());
}

void	HTTPObject::setConfig(Server config)
{
	_request->setConfig(config);
	_request->configConfig();
	_config = _request->getConfig();
}

void	HTTPObject::setReadFd(int fd)
{
	_readFd = fd;
}

void	HTTPObject::setWriteFd(int fd)
{
	_writeFd = fd;
}

int	HTTPObject::getReadFd()
{
	return (_readFd);
}

int	HTTPObject::getWriteFd()
{
	return (_writeFd);
}

int		HTTPObject::getClientFd()
{
	return (_clientFd);
}
