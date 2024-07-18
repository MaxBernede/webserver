#include "webserver.hpp"

void ServerRun::sendResponse(int fd)
{
		std::cout << "SENDING RESPONSE" << std::endl;
		Logger::log("fd: " + std::to_string(fd), WARNING);
		int clientFd = _requests[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		removeConnection(fd);
		if (_responses.count(clientFd))
		{
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(fd))
		{
			delete _requests[fd];
			_requests.erase(fd);
		}
		if (_requests.count(clientFd))
		{
			_requests.erase(clientFd);
		}
		if (!_responses.count(clientFd) && !_requests.count(fd))
		{
			close(clientFd); // only loads in the browser one the fd is closed...should we keep the connection?
			removeConnection(clientFd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendCgiResponse(int fd)
{
		std::cout << "SENDING CGI RESPONSE" << std::endl;
		int clientFd = _cgi[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connectioned?
		removeConnection(fd);
		if (_cgi.count(fd))
		{
			delete _cgi[fd];
			_cgi.erase(fd);
		}
		if (_responses.count(clientFd))
		{
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(clientFd))
		{
			delete _requests[clientFd];
			_requests.erase(clientFd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendError(int clientFd)
{
	Logger::log("Sending Redir msg", INFO);
	//std::cout << "SENDING REDIR ERROR" << std::endl;
	Response *r = _responses[clientFd];
	r->rSend();
	close(clientFd);
	if (_responses.count(clientFd))
	{
		delete _responses[clientFd];
		_responses.erase(clientFd);
	}
	if (_requests.count(clientFd) == 1)
	{
		delete _requests[clientFd];
		_requests.erase(clientFd);
	}
	_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY; // But did I not close this?
}

void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd)
{
	//Logger::log(std::to_string(pollData._pollType), INFO);
	switch (pollData._pollType)
	{
		case CGI_READ_DONE:
			sendCgiResponse(pollFd.fd);
			break ;
		case FILE_READ_DONE:
			sendResponse(pollFd.fd);
			break ;
		case SEND_REDIR:
			sendError(pollFd.fd);
		default:
			break ;
	}
}
