#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "CGI.hpp"

const std::string HTTP_CONFLICT_RESPONSE = R"(
HTTP/1.1 409 Conflict
Content-Type: application/json

{
    "error": "Conflict",
    "message": "The request could not be completed due to a conflict with the current state of the target resource."
}
)";

const std::string HTTP_FORBIDDEN_RESPONSE = R"(
HTTP/1.1 403 Forbidden
Content-Type: application/json

{
    "error": "Forbidden",
    "message": "You do not have permission to access this resource."
}
)";

ServerRun::ServerRun(const std::list<Server> config)
{
	std::vector<int> listens;

	if (config.empty())
		throw(Exception("No servers defined in the config file", 1));
	_servers = config;
	// looping over the sever bloacks
	for (auto server : _servers)
	{
		for (auto port : server.getPorts())
		{
			if (std::find(listens.begin(), listens.end(), port.nmb) == listens.end())
				listens.push_back(port.nmb);
			else
			{
				std::cout << "port: " << port.nmb << std::endl;
				std::cout << "Servers have the same port in config" << std::endl;
			}
		}
	}
	//create listening sockets
	createListenerSockets(listens);
}

ServerRun::~ServerRun( void )
{
	// delete everything
	for (auto it : _listenSockets)
	{
		close(it->getFd());
		delete it;
	}
}

void ServerRun::createListenerSockets(std::vector<int> listens)
{
	Socket *new_socket;
	for (auto listen : listens)
	{
		try
		{
			new_socket = new Socket(listen);
			_listenSockets.push_back(new_socket);
		}
		catch (const Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	if (_listenSockets.empty())
		throw(Exception("No available port on the defined host", 1));
	// add listener sockets to queue
	for (int i = 0; i < (int)_listenSockets.size(); i++)
	{
		addQueue(LISTENER, _listenSockets[i]->getFd());
	}
}

void ServerRun::addQueue(pollType type, int fd)
{
	s_poll_data newPollItem;
	struct pollfd newPollFd;

	newPollFd = {fd, POLLIN | POLLOUT, 0};
	newPollItem._pollType = type;
	_pollFds.push_back(newPollFd);
	_pollData[fd] = newPollItem;
}

void ServerRun::serverRunLoop( void )
{
	int nCon = -1;
	Logger::log("Server running... ", INFO);
	while (true)
	{
		nCon = poll(_pollFds.data(), _pollFds.size(), 0);
		if (nCon <= 0)
		{
			if (nCon < 0 and errno != EAGAIN) // EGAIN: Resource temporarily unavailable
				throw(Exception("Poll failed", errno));
			continue ;
		}
		for (int i = 0; i < (int)_pollFds.size(); i++)
		{
			int fd = _pollFds[i].fd;
			try
			{
				if (_pollFds[i].revents & POLLIN)
				{
					// Only start reading CGI once the write end of the pipe is closed
					if ((_pollFds[i].revents & POLLHUP) && _pollData[fd]._pollType == CGI_READ_WAITING)
					{
						std::cout << "CGI write side finished writing to the pipe\n";
						_pollData[fd]._pollType = CGI_READ_READING;
					}
					//Read from client
					dataIn(_pollData[fd], _pollFds[i]);
				}
				if (_pollFds[i].revents & POLLOUT || _pollData[fd]._pollType == CGI_READ_DONE)
				{
					// Write to client
					dataOut(_pollData[fd], _pollFds[i]);
				}
			}
			catch(const Exception& e)
			{
				//Cath of the "Throw Port not found" in the readRequest;
				//std::cerr << e.what() << '\n';
				;
			}
		}
	}
}

void ServerRun::acceptNewConnection(int listenerFd)
{
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	std::cout << "New Client Connected accepted, CONN: " << connFd << std::endl;
	if (connFd == -1)
		throw(Exception("accept() errored and returned -1", errno));
	addQueue(CLIENT_CONNECTION_READY, connFd);
}

Server ServerRun::getConfig(int port) // WILL ADD HOST
{
	for (auto server : _servers)
	{
		for (auto p : server.getPorts())
		{
			if (p.nmb == port)
			{
				return (server);
			}
		}
	}
	throw(Exception("Server not found", 1));
}

Server ServerRun::getConfig(std::string host)
{
	for (auto server : _servers)
	{
		if (server.getName() == host)
		{
			return (server);
		}
	}
	throw Exception("Server not found", 1);
}

void ServerRun::handleCGIRequest(int clientFd)
{
	std::cout << "CGI Request\n";
	CGI *cgiRequest = new CGI(_requests[clientFd], clientFd);
	int pipeFd = cgiRequest->getReadFd();
	_cgi[pipeFd] = cgiRequest;
	// std::cout << "Cgi Pipe FD: " << pipeFd << std::endl;
	addQueue(CGI_READ_WAITING, pipeFd);
	cgiRequest->runCgi();
}

void ServerRun::handleStaticFileRequest(int clientFd)
{
	// TODO check if _requests[clientFd]->getFileName() is defined in the configs redirect
	std::string filePath = _requests[clientFd]->getConfig().getRoot() + "html/" + _requests[clientFd]->getFileName(); // TODO root path based on config
	std::cout << "Opening static file: " << filePath << std::endl;
	int fileFd = open(filePath.c_str(), O_RDONLY);
	if (fileFd < 0)
	{
		std::cout << "Failed opening file: " << filePath << std::endl; // TODO 404 error
		throw(Exception("Opening static file failed", errno));
	}
	Logger::log("File correctly opened", INFO);
	_requests[fileFd] = _requests[clientFd]; // TODO: We need to add the request header reading in here too
	addQueue(FILE_READ_READING, fileFd);
}

void ServerRun::redirectToError(int ErrCode, Request *request, int clientFd)
{
	//check this code to implement the no reply 204 case of successfull delete or stuff like that
	if (_responses.find(clientFd) == _responses.end()) 
	{
		Response *response = new Response(request, clientFd, true);
		if (ErrCode == 404)
			response->setResponseString(NOT_FOUND);
		if (ErrCode == 405)
			response->setResponseString(NOT_ALLOWED);
		if (ErrCode == NO_CONTENT)
			response->setResponseString("HTTP/1.1 204 No Content");
		if (ErrCode == ErrorCode::CONFLICT)
			response->setResponseString(HTTP_CONFLICT_RESPONSE);
		if (ErrCode == ErrorCode::FORBIDDEN)
			response->setResponseString(HTTP_FORBIDDEN_RESPONSE);
		_responses[clientFd] = response;
		_pollData[clientFd]._pollType = SEND_REDIR;
	}
}

// Only continue after reading the whole request
void ServerRun::readRequest(int clientFd)
{	
	if (_requests.find(clientFd) == _requests.end())
	{
		Request *newRequest = new Request(clientFd);
		_requests[clientFd] = newRequest;
	}
	else if (_requests[clientFd]->isDoneReading() == false)
	{
		_requests[clientFd]->readRequest();
	}
	if (_requests[clientFd]->isDoneReading() == true)
	{
		int port = _requests[clientFd]->getRequestPort();
		if (port < 0)
		{
			throw Exception("Port not found", errno);
			exit(1);
		}
		Server config = getConfig(port);
		//TODO if server == not found, error should be thrown, please catch
		_requests[clientFd]->setConfig(config);
		int ErrCode = _requests[clientFd]->getErrorCode();
		if (ErrCode != 200)
			ErrCode = _requests[clientFd]->checkRequest(); // Max code : this is a request.getErrorCode();
		//if (ErrCode != 0) //Yesim code
		if (ErrCode != 200)
		{
			_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
			redirectToError(ErrCode, _requests[clientFd], clientFd);
			return ;
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
		if (_requests[clientFd]->isCgi()) // What do we do when CGI is not allowed?
		{
			if (!config.getCGI())
			{
				std::cout << "CGI is not allowed for this server\n";
				return ;
			}
			handleCGIRequest(clientFd);
		}
		else // Static file
		{
			// if (_requests[clientFd]->getMethod(0) == "HEAD")
			// 	Logger::log("Method doesn't read because HEAD", INFO);
			// else
			handleStaticFileRequest(clientFd);
		}
		_requests.erase(clientFd);
	}
}
void ServerRun::removeConnection(int fd)
{
	for (int i = 0; i < (int)_pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break ;
		}
	}
	if (_pollData.count(fd))
		_pollData.erase(fd);
}

void ServerRun::readFile(int fd) // Static file fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	int clientFd = _requests[fd]->getClientFd();
	if (_responses.find(clientFd) == _responses.end()) // Response object not created
	{
		Response *response = new Response(_requests[fd], clientFd, false);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read file failed", errno));
	if (readChars > 0)
	{
		_responses[clientFd]->addToBuffer(std::string(buffer, readChars));
	}
	if (readChars == 0)
	{
		_pollData[fd]._pollType = FILE_READ_DONE;
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::readPipe(int fd) // Pipe read end fd
{
	char buffer[BUFFER_SIZE];

	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = '\0';
	int clientFd = _cgi[fd]->getClientFd();
	if (_responses.find(clientFd) == _responses.end()) // Response object not created
	{
		Response *response = new Response(_cgi[fd]->getRequest(), clientFd, false);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read pipe failed!", errno));
	if (readChars > 0)
	{
		_responses[clientFd]->addToBuffer(std::string(buffer, readChars));
	}
	if (readChars < BUFFER_SIZE - 1)
	{
		_pollData[fd]._pollType = CGI_READ_DONE;
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{
	// std::cout << "Poll FD: " << pollFd.fd << " Poll type: " << pollData.pollType << std::endl;
	switch (pollData._pollType)
	{
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

void ServerRun::sendResponse(int fd)
{
		std::cout << "SENDING RESPONSE" << std::endl;
		int clientFd = _requests[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		removeConnection(fd);
		if (_responses.count(clientFd) == 1)
		{
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(fd) == 1)
		{
			delete _requests[fd];
			_requests.erase(fd);
		}
		if (!_responses.count(clientFd) and !_requests.count(fd))
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
		if (_requests.count(clientFd) == 1)
		{
			delete _requests[clientFd];
			_requests.erase(clientFd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendRedir(int clientFd)
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
			sendRedir(pollFd.fd);
		default:
			break ;
	}
}

/*

vector requests
vector responses (response string)
vector cgi

CLIENT_CONNECTION (write here) --> read to see the request / write to response
CGI_PIPE_READ (read html file) --> to create the response string
FILE_READ (read html file) --> to create the response string
FILE_WRITE (needed for post method?)
LISTENER_SOCKET (read to see new connection requests)

*/
