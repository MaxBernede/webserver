#include "HTTPObject.hpp"

HTTPObject::HTTPObject(int clientFd) : 
				_clientFd(clientFd),
				_cgi(NULL)
{
	_request = new Request(clientFd);
	_response = new Response(clientFd);
}

HTTPObject::~HTTPObject(void)
{
	delete _request;
	delete _response;
	if (_cgi != NULL)
	{
		delete _cgi;
	}
}

std::string	HTTPObject::redirectResponse(void)
{
	std::string oss = _response->redirectResponse(_clientFd, _request->getFileNameProtected(), _request->getConfig().getRedirect());
	return (oss);
}

void	HTTPObject::createCGI()
{
	_cgi = new CGI(_request, _clientFd);
}

void	HTTPObject::runCGI(void)
{
	_cgi->run();
}

void	HTTPObject::sendResponse(void)
{
	_response->rSend(_request);
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

int	HTTPObject::getReadFd(void)
{
	return (_readFd);
}

bool	HTTPObject::isCgi(void)
{
	return (_request->isCgi());
}

int		HTTPObject::getClientFd(void)
{
	return (_clientFd);
}
