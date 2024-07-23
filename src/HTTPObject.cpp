#include "webserver.hpp"

HTTPObject::HTTPObject(int clientFd) : 
				_clientFd(clientFd),
				_isCgi(false)
{
	_request = new Request(clientFd);
	_response = new Response();
}

HTTPObject::~HTTPObject(void){}

void	HTTPObject::setConfig(Server config)
{
	_config = config;
	_request->setConfig(config);
}

std::string HTTPObject::redirectResponse(void)
{
	std::string oss = _reponse.redirectResponse(_clientFd, _request->getFileNameProtected(), _request->getConfig().getRedirect());
	return (oss);
}

void	HTTPObject::sendResponse(void)
{
	_reponse.rSend(_request);
}

bool	HTTPObject::isCgi(void)
{
	return (_isCgi);
}

int		HTTPObject::getClientFd(void)
{
	return (_clientFd);
}
