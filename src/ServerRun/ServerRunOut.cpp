#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "CGI.hpp"

void ServerRun::sendCgiResponse(int fd){
		std::cout << "SENDING CGI RESPONSE" << std::endl;
		int clientFd = _cgi[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connectioned?
		removeConnection(fd);
		if (_cgi.count(fd)){
			delete _cgi[fd];
			_cgi.erase(fd);
		}
		if (_responses.count(clientFd)){
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(fd)){
			delete _requests[fd];
			_requests.erase(fd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendResponse(int fd){
		std::cout << "SENDING RESPONSE" << std::endl;
		int clientFd = _requests[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		removeConnection(fd);
		// _requests.erase(clientFd);
		if (_responses.count(clientFd) == 1){
			std::cout << "1\n";
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(fd) == 1){
			std::cout << "2\n";
			delete _requests[fd];
			_requests.erase(fd);
		}
		if (!_responses.count(clientFd) and !_requests.count(fd)){
			std::cout << "3\n";
			close(clientFd); // only loads in the browser one the fd is closed...should we keep the connection?
			removeConnection(clientFd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendRedirect(int fd){
	std::cout << "SENDING HTTP REDIRECT" << std::endl;
	int clientFd = _requests[fd]->getClientFd();
	Response *r = _responses[clientFd];
	r->redirectResponse();
	close(clientFd);
	removeConnection(fd);
	if (_responses.count(clientFd) == 1){
		std::cout << "1\n";
		delete _responses[clientFd];
		_responses.erase(clientFd);
	}
	if (_requests.count(fd) == 1){
		std::cout << "2\n";
		delete _requests[fd];
		_requests.erase(fd);
	}
	if (!_responses.count(clientFd) and !_requests.count(fd)){
		std::cout << "3\n";
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connection?
		removeConnection(clientFd);
	}
	_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd){
	switch (pollData._pollType){
		case CGI_READ_DONE:
			sendCgiResponse(pollFd.fd);
			break ;
		case FILE_READ_DONE:
			sendResponse(pollFd.fd);
			break ;
		case HTTP_REDIRECT:
			sendRedirect(pollFd.fd);
			break ;
		default:
			break ;
	}
}
