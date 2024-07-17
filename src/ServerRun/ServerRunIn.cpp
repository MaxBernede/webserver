#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "CGI.hpp"
#include<Error.hpp>

void ServerRun::acceptNewConnection(int listenerFd){
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	std::cout << "New Client Connected accepted, CONN: " << connFd << std::endl;
	if (connFd == -1)
		throw(Exception("accept() errored and returned -1", errno));
	addQueue(CLIENT_CONNECTION_READY, connFd);
}

void ServerRun::handleCGIRequest(int clientFd){
	std::cout << "CGI Request\n";
	CGI *cgiRequest = new CGI(_requests[clientFd], clientFd);
	int pipeFd = cgiRequest->getReadFd();
	_cgi[pipeFd] = cgiRequest;
	// std::cout << "Cgi Pipe FD: " << pipeFd << std::endl;
	addQueue(CGI_READ_WAITING, pipeFd);
	cgiRequest->runCgi();
}

void ServerRun::handleStaticFileRequest(int clientFd){
	// TODO check if _requests[clientFd]->getFileName() is defined in the configs redirect
	std::string filePath = _requests[clientFd]->getConfig().getRoot() + _requests[clientFd]->getFileName(); // TODO root path based on config
	std::cout << "Opening static file: " << filePath << std::endl;
	int fileFd = open(filePath.c_str(), O_RDONLY);
	if (fileFd < 0)
	{
		std::cout << "Failed opening file: " << filePath << std::endl; // TODO 404 error
		throw(Exception("Opening static file failed", errno));
	}
	_requests[fileFd] = _requests[clientFd]; // TODO: We need to add the request header reading in here too
	addQueue(FILE_READ_READING, fileFd);
}

void ServerRun::redirectToError(int ErrCode, Request *request, int clientFd){
	if (_responses.find(clientFd) == _responses.end()) 
	{
		Response *response = new Response(request, clientFd, true);
		if (ErrCode == 404)
			response->setResponseString(NOT_FOUND);
		if (ErrCode == 405)
			response->setResponseString(NOT_ALLOWED);
		_responses[clientFd] = response;
		_pollData[clientFd]._pollType = SEND_REDIR;
	}
}

// Only continue after reading the whole request
void ServerRun::readRequest(int clientFd){	
	if (_requests.find(clientFd) == _requests.end()){
		Request *newRequest = new Request(clientFd);
		_requests[clientFd] = newRequest;
	}
	else if (_requests[clientFd]->isDoneReading() == false){
		_requests[clientFd]->readRequest();
	}
	if (_requests[clientFd]->isDoneReading() == true){
		s_domain Domain = _requests[clientFd]->getRequestDomain();
		Server config = getConfig(Domain);
		_requests[clientFd]->setConfig(config);
		_requests[clientFd]->configConfig();
		int ErrCode = _requests[clientFd]->checkRequest();
		if (ErrCode != 0){
			_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
			redirectToError(ErrCode, _requests[clientFd], clientFd);
			return ;
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
		if (_requests[clientFd]->isRedirect()){
			printColor(RED, "HTTP REDIRECT\n");
			int temp = dup(clientFd);
			_requests[temp] = _requests[clientFd];
			if (_responses.find(clientFd) == _responses.end()) {
				Response *response = new Response(_requests[temp], clientFd, false);
				_responses[clientFd] = response;
			}
			addQueue(HTTP_REDIRECT, temp);
		}
		else {
			if (_requests[clientFd]->isCgi()) {
				if (!config.getCGI()){
					std::cout << "CGI is not allowed for this server\n"; // TODO: send some sort of error
					return ;
				}
				handleCGIRequest(clientFd);
			}
			else { // Static file
				handleStaticFileRequest(clientFd);
			}
		}
	// 	std::string body = _requests[clientFd]->getValues("Body");
	// 	// std::cout << _requests[clientFd]->getConfig() << std::endl;
	// 	if (!body.empty()){
	// 		printColor(RED, "BODY CREATE");
	// 		create_file(body, _requests[clientFd]->getConfig().getRoot());
	// 	}
		_requests.erase(clientFd);
	}
}

// Static file fd
void ServerRun::readFile(int fd) {
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	int clientFd = _requests[fd]->getClientFd();
	// Response object not created
	if (_responses.find(clientFd) == _responses.end()) {
		Response *response = new Response(_requests[fd], clientFd, false);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read file failed", errno));
	if (readChars > 0){
		_responses[clientFd]->addToBuffer(std::string(buffer, readChars));
	}
	if (readChars == 0){
		_pollData[fd]._pollType = FILE_READ_DONE;
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::readPipe(int fd) // Pipe read end fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	int clientFd = _cgi[fd]->getClientFd();
	// Response object not created
	if (_responses.find(clientFd) == _responses.end()){
		Response *response = new Response(_cgi[fd]->getRequest(), clientFd, false);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read pipe failed!", errno));
	if (readChars > 0){
		_responses[clientFd]->addToBuffer(std::string(buffer, readChars));
	}
	if (readChars < BUFFER_SIZE - 1){
		_pollData[fd]._pollType = CGI_READ_DONE;
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd){
	// std::cout << "Poll FD: " << pollFd.fd << " Poll type: " << pollData.pollType << std::endl;
	switch (pollData._pollType){
		case LISTENER:
			acceptNewConnection(pollFd.fd);
			break ;
		case CLIENT_CONNECTION_READY: 
			readRequest(pollFd.fd);
			break ;
		case CGI_READ_READING:
			readPipe(pollFd.fd);
			break ;
		case FILE_READ_READING:
			readFile(pollFd.fd);
			break ;
		default:
			break ;
	}
}
