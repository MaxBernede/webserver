#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "CGI.hpp"

void ServerRun::acceptNewConnection(int listenerFd){
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	std::cout << "New Client Connected accepted\n";
	if (connFd == -1)
		throw(Exception("accept() errored and returned -1", errno));
	addQueue(CLIENT_CONNECTION_READY, connFd);
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
		// TODO if host/port/server are not found, exception is thrown,
		// should be caught and 404 should be sent
		// int port = _requests[clientFd]->getRequestPort();
		s_port hostPort = _requests[clientFd]->getRequestHostPort();
		Server config = getConfig(hostPort);
		// Server config = getConfig(port);
		_requests[clientFd]->setConfig(config);
		_requests[clientFd]->configConfig();
		_requests[clientFd]->checkRequest();
		std::cout << "ROOT directory:\t"
			<< _requests[clientFd]->getConfig().getRoot() << std::endl;
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
		if (_requests[clientFd]->isRedirect()){
			// not working yet, apparently, it doesn't send properly :/
			printColor(RED, "HTTP REDIRECT\n");
			Response r(_requests[clientFd], clientFd);
			r.redirectResponse();
			// addQueue(HTTP_REDIRECT, clientFd);
		}
		else {
			if (_requests[clientFd]->isCgi()) {
				if (!config.getCGI()){
					std::cout << "CGI is not allowed for this server\n"; // TODO: send some sort of error
					return ;
				}
				std::cout << "It is a CGI Request!\n";
				CGI *cgiRequest = new CGI(_requests[clientFd], clientFd);
				int pipeFd = cgiRequest->getReadFd();
				_cgi[pipeFd] = cgiRequest;
				// std::cout << "Cgi Pipe FD: " << pipeFd << std::endl;
				addQueue(CGI_READ_WAITING, pipeFd);
				cgiRequest->runCgi();
			}
			else { // Static file
				std::string filePath = _requests[clientFd]->getConfig().getRoot() + _requests[clientFd]->getFileName();
				std::cout << "opening file: " << filePath << std::endl;
				int fileFd = open(filePath.c_str(), O_RDONLY);
				if (fileFd < 0){
					std::cout << "Failed opening file: " << filePath << std::endl; // TODO 404 error
					throw(Exception("Opening static file failed", errno));
				}
				_requests[fileFd] = _requests[clientFd]; // TODO: We need to add the request header reading in here too
				addQueue(FILE_READ_READING, fileFd);
			}
		}
		std::string body = _requests[clientFd]->getValues("Body");
		// std::cout << _requests[clientFd]->getConfig() << std::endl;
		if (!body.empty()){
			printColor(RED, "BODY CREATE");
			create_file(body, _requests[clientFd]->getConfig().getRoot());
		}
		_requests.erase(clientFd);
	}
}

void ServerRun::readPipe(int fd) // Pipe read end fd
{
	char buffer[BUFFER_SIZE];

	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = '\0';
	int clientFd = _cgi[fd]->getClientFd();
	// Response object not created
	if (_responses.find(clientFd) == _responses.end()){
		Response *response = new Response(_cgi[fd]->getRequest(), clientFd);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	std::cout << "Cgi read chars " << readChars << std::endl;
	if (readChars < 0)
		throw(Exception("Read pipe failed!", errno));
	if (readChars > 0){
		std::cout << "Cgi: adding to buffer\n";
		_responses[clientFd]->addToBuffer(buffer);
	}
	if (readChars < BUFFER_SIZE - 1){
		_pollData[fd]._pollType = CGI_READ_DONE;
		std::cout << "Setting CGI READ DONE\n";
		_responses[clientFd]->setReady();
		close(fd);
	}
}

// Static file fd
void ServerRun::readFile(int fd) {
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	int clientFd = _requests[fd]->getClientFd();
	// Response object not created
	if (_responses.find(clientFd) == _responses.end()) {
		Response *response = new Response(_requests[fd], clientFd);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read file failed", errno));
	if (readChars > 0){
		_responses[clientFd]->addToBuffer(buffer);
	}
	if (readChars == 0){
		_pollData[fd]._pollType = FILE_READ_DONE;
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
