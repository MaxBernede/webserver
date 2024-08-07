#include "HTTPObject.hpp"

HTTPObject::HTTPObject(int clientFd) :
	_clientFd(clientFd),
	_startTime(std::chrono::high_resolution_clock::now()),
	_cgi(NULL)
{
	_request = new Request(clientFd);
	_response = new Response(clientFd);
	_timeOut = false;
	_totalBytesWritten = 0;
	_writeFinished = false;
}

HTTPObject::~HTTPObject()
{
	delete _request;
	delete _response;
	if (_cgi != NULL)
		delete _cgi;
}
void	HTTPObject::sendAutoIndex()
{
	_response->rSend();
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
	std::string body = _request->getRawBody(); // Raw body with boundary strings
	ssize_t bytesWritten;
	if (_totalBytesWritten < body.length())
	{
		bytesWritten = write(_writeFd, body.c_str() + _totalBytesWritten, body.length() - _totalBytesWritten);
		if (bytesWritten == -1)
			throw(Exception("Writing to pipe failed", 1));
		_totalBytesWritten += bytesWritten;
	}
	else if (_totalBytesWritten == body.length())
	{
		_writeFinished = true;
	}
}

void	HTTPObject::runCgi()
{
	_cgi->run();
}

void	HTTPObject::sendResponseWithHeaders()
{
	_response->addHeaders(_request);
	_response->rSend();
}

bool	HTTPObject::isCgi()
{
	return (_request->isCgi());
}

void	HTTPObject::checkTimeOut()
{
	std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>
		(std::chrono::high_resolution_clock::now() - _startTime);
	std::chrono::seconds ten(10);
	if (sec > ten) {
		_timeOut = true;
		if (this->isCgi())
			this->_cgi->killChild();
		throw (HTTPError(GATEWAY_TIMEOUT));
	}
}

void	HTTPObject::setConfig()
{
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

TimePoint HTTPObject::getStartTime()
{
	return _startTime;
}

bool	HTTPObject::getTimeOut()
{
	return (_timeOut);
}

bool	HTTPObject::getWriteFinished()
{
	return (_writeFinished);
}