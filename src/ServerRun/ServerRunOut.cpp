#include "webserver.hpp"

void ServerRun::sendResponse(int fd)
{
		Logger::log("Sending response to fd: " + std::to_string(fd), WARNING);
		// int clientFd = _requests[fd]->getClientFd();
		// Response *r = _responses[clientFd];
		// r->rSend();
		HTTPObject *obj = findHTTPObject(fd);
		obj->sendResponse();
		removeConnection(fd);
		int clientFd = obj->getClientFd();
		close(clientFd);
		if (_httpObjects.count(clientFd))
		{
			delete _httpObjects[clientFd];
			_httpObjects.erase(clientFd);
		}
		// if (_responses.count(clientFd))
		// {
		// 	delete _responses[clientFd];
		// 	_responses.erase(clientFd);
		// }
		// if (_requests.count(fd))
		// {
		// 	delete _requests[fd];
		// 	_requests.erase(fd);
		// }
		// if (_requests.count(clientFd))
		// {
		// 	_requests.erase(clientFd);
		// }
		// if (!_responses.count(clientFd) && !_requests.count(fd))
		// {
		// 	close(clientFd); // only loads in the browser one the fd is closed...should we keep the connection?
		// 	removeConnection(clientFd);
		// }
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendCgiResponse(int fd)
{
		std::cout << "SENDING CGI RESPONSE" << std::endl;
		// int clientFd = _cgi[fd]->getClientFd();
		// Response *r = _responses[clientFd];
		// r->rSend();
		HTTPObject *obj = findHTTPObject(fd);
		obj->sendResponse();
		removeConnection(fd);
		int clientFd = obj->getClientFd();
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connectioned?
		if (_httpObjects.count(clientFd))
		{
			delete _httpObjects[clientFd];
			_httpObjects.erase(clientFd);
		}
		// if (_cgi.count(fd))
		// {
		// 	delete _cgi[fd];
		// 	_cgi.erase(fd);
		// }
		// if (_responses.count(clientFd))
		// {
		// 	delete _responses[clientFd];
		// 	_responses.erase(clientFd);
		// }
		// if (_requests.count(clientFd))
		// {
		// 	_requests.erase(clientFd);
		// }
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendError(int clientFd)
{
	Logger::log("Sending Redir msg", INFO);
	//std::cout << "SENDING REDIR ERROR" << std::endl;
	// Response *r = _responses[clientFd];
	// r->rSend();
	_httpObjects[clientFd]->sendResponse();
	close(clientFd);
	if (_httpObjects.count(clientFd))
	{
		delete _httpObjects[clientFd];
		_httpObjects.erase(clientFd);
	}
	// if (_responses.count(clientFd))
	// {
	// 	delete _responses[clientFd];
	// 	_responses.erase(clientFd);
	// }
	// if (_requests.count(clientFd) == 1)
	// {
	// 	delete _requests[clientFd];
	// 	_requests.erase(clientFd);
	// }
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
			break ;
		case HTTP_REDIRECT:
			sendRedirect(pollFd.fd);
			break ;
		default:
			break ;
	}
}

void ServerRun::sendRedirect(int fd){
	std::cout << "SENDING HTTP REDIRECT" << std::endl;
	// int clientFd = _requests[fd]->getClientFd();
	// Response *r = _responses[clientFd];
	// r->redirectResponse();
	HTTPObject *obj = findHTTPObject(fd);
	obj->redirectResponse();
	removeConnection(fd);
	int clientFd = obj->getClientFd();
	close(clientFd);
	if (_httpObjects.count(clientFd))
	{
		delete _httpObjects[clientFd];
		_httpObjects.erase(clientFd);
	}
	// if (_responses.count(clientFd) == 1){
	// 	delete _responses[clientFd];
	// 	_responses.erase(clientFd);
	// }
	// if (_requests.count(fd) == 1){
	// 	delete _requests[fd];
	// 	_requests.erase(fd);
	// }
	// if (!_responses.count(clientFd) and !_requests.count(fd)){
	// 	close(clientFd);
	// 	removeConnection(clientFd);
	// }
	_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}