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

void	HTTPObject::writeToCgiPipe()
{
	std::string body = _request->getValues("Body");
	Logger::log("Body: " + body, LogLevel::INFO);
	if (body.length())
	{
		if (write(_writeFd, body.c_str(), body.length()))
		{
			throw(Exception("Write failed", 1));
		}
	}
	_doneWritingToCgi = true; // Maybe write in chunks? For now this variable has no use...
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
