#include "ServerRun.hpp"

void ServerRun::sendResponse(int fd) // Using readFd
{
		Logger::log("Sending response to fd: " + std::to_string(fd), WARNING);
		HTTPObject *obj = findHTTPObject(fd);
		obj->sendResponse();
		removeConnection(fd);
		int clientFd = obj->getClientFd();
		cleanUp(clientFd);
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendRedirect(int clientFd) // this is a clientFd!
{
	_httpObjects[clientFd]->sendRedirection();
	cleanUp(clientFd);
	_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendError(int clientFd)
{
	//Logger::log("Sending Redir msg", INFO);
	_httpObjects[clientFd]->sendResponse();
	cleanUp(clientFd);
	_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY; // But did I not close this?
}

// Sending data from the server to the client
void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd)
{
	switch (pollData._pollType)
	{
		case CGI_READ_DONE:
			sendResponse(pollFd.fd);
			break ;
		case FILE_READ_DONE:
			sendResponse(pollFd.fd);
			break ;
		case EMPTY_RESPONSE:
			sendError(pollFd.fd);
			break;
		case HTTP_ERROR:
			sendError(pollFd.fd);
			break ;
		case HTTP_REDIRECT:
			sendRedirect(pollFd.fd);
			break ;
		default:
			break ;
	}
}

void ServerRun::cleanUp(int clientFd)
{
	close(clientFd);
	if (_httpObjects.count(clientFd))
	{
		delete _httpObjects[clientFd];
		_httpObjects.erase(clientFd);
	}
}
