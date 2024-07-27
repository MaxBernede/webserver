#include "ServerRun.hpp"

void ServerRun::acceptNewConnection(int listenerFd)
{
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	if (connFd == -1)
		throw (Exception("Error: accept() failed and returned -1", errno));
	Logger::log("New client connection accepted at fd: " + std::to_string(connFd), LogLevel::DEBUG);
	addQueue(CLIENT_CONNECTION_READY, CLIENTFD, connFd);
}

void ServerRun::handleCGIRequest(int clientFd)
{
	Logger::log("A CGI Request is being handled", LogLevel::INFO);
	_httpObjects[clientFd]->createCGI();
	int pipeFd = _httpObjects[clientFd]->_cgi->getReadFd();
	_httpObjects[clientFd]->setReadFd(pipeFd);
	addQueue(CGI_READ_WAITING, READFD, pipeFd);
	_httpObjects[clientFd]->runCGI();
}

void ServerRun::handleStaticFileRequest(int clientFd)
{
	std::string filePath = _httpObjects[clientFd]->_request->getFilePath();
	std::cout << "Opening static file: " << filePath << std::endl;
	int fileFd = open(filePath.c_str(), O_RDONLY);
	if (fileFd < 0)
	{
		std::cout << "Failed opening file: " << filePath << std::endl; // TODO 404 error
		throw (Exception("Opening static file failed", errno));
	}
	Logger::log("File correctly opened", INFO);
	_httpObjects[clientFd]->setReadFd(fileFd);
	addQueue(FILE_READ_READING, READFD, fileFd);
}

// Handles error code when no error file exists
void ServerRun::redirectToError(ErrorCode ErrCode, int clientFd)
{
	Logger::log("Redirecting to Error...", LogLevel::WARNING);
	// need to add file search here...

	HTTPObject *obj;
	if (_pollData[clientFd]._fdType == CLIENTFD)
		obj = _httpObjects[clientFd];
	else
		obj = findHTTPObject(clientFd);
	obj->_request->searchErrorPage();
	if (ErrCode == HTTP_NOT_SUPPORT){
		_pollData[clientFd]._pollState = HTTP_ERROR;
	}
	else if (obj->_request->getErrorPageStatus() == false) // if no error file does not exst
	{
		Logger::log("Error page does not exist..Error code: " + std::to_string(ErrCode), LogLevel::WARNING);
		obj->_response->errorResponseHTML(ErrCode);
		std::cout << "created response string\n";
		// if (ErrCode == NO_CONTENT)
		// 	obj->_response->setResponseString("HTTP/1.1 204 No Content");
		_pollData[clientFd]._pollState = HTTP_ERROR;
	}
	else // if error page exists
	{
		handleStaticFileRequest(clientFd);
	}
}

int ServerRun::httpRedirect(ErrorCode status, int clientFd)
{
	HTTPObject *obj = _httpObjects[clientFd];
	int Err = obj->_response->setRedirectStr(status, obj->_request->getFileNameProtected(), obj->_config.getRedirect());
	Logger::log("HTTP Error Caught, Redirection Detected", LogLevel::DEBUG);
	return (Err);
}

// Only continue after reading the whole request
void ServerRun::handleRequest(int clientFd)
{
	if (_httpObjects.find(clientFd) == _httpObjects.end())
	{
		Logger::log("Creating HTTP Obj", LogLevel::INFO);
		HTTPObject *newObj = new HTTPObject(clientFd);
		_httpObjects[clientFd] = newObj;
	}
	if (_httpObjects[clientFd]->_request->isDoneReading() == false)
	{
		_httpObjects[clientFd]->_request->readRequest();
	}
	if (_httpObjects[clientFd]->_request->isDoneReading() == true)
	{
		_httpObjects[clientFd]->_request->startConstruRequest();
		s_domain Domain = _httpObjects[clientFd]->_request->getRequestDomain();
		Server config = findConfig(Domain);
		_httpObjects[clientFd]->setConfig(config);
		_httpObjects[clientFd]->_request->checkRequest();
		_pollData[clientFd]._pollState = CLIENT_CONNECTION_WAIT;
		executeRequest(clientFd, config);
	}
}

void ServerRun::executeRequest(int clientFd, Server config){
	if (_httpObjects[clientFd]->isCgi()) // GET and POST for CGI
	{
		if (!config.getCGI())
		{
			Logger::log("CGI is not permitted for this server", LogLevel::ERROR);
			throw (HTTPError(ErrorCode::FORBIDDEN)); // What do we do when CGI is not allowed?
		}
		handleCGIRequest(clientFd);
	}
	else if (_httpObjects[clientFd]->_request->isEmptyResponse()) { 	// POST or DEL or HEAD
		_pollData[clientFd]._pollState = EMPTY_RESPONSE;
		
		_httpObjects[clientFd]->_request->execAction();
	}
	else // Static file
	{
		handleStaticFileRequest(clientFd);
	}
}

void ServerRun::readFile(int fd) // Static file fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	HTTPObject *obj = findHTTPObject(fd);
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read file failed", errno));
	if (readChars > 0)
	{
		obj->_response->addToBuffer(std::string(buffer, readChars));
	}
	if (readChars == 0)
	{
		_pollData[fd]._pollState = FILE_READ_DONE;
		obj->_response->setReady();
		close(fd);
	}

}

void ServerRun::readPipe(int fd) // Pipe read-end fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	HTTPObject *obj = findHTTPObject(fd);
	if (obj->_cgi->waitCgiChild())
	{
		int readChars = read(fd, buffer, BUFFER_SIZE - 1);
		if (readChars < 0)
			throw(Exception("Read pipe failed!", errno));
		if (readChars > 0)
		{
			obj->_response->addToBuffer(std::string(buffer, readChars));
		}
		if (readChars < BUFFER_SIZE - 1)
		{
			_pollData[fd]._pollState = CGI_READ_DONE;
			obj->_response->setReady();
			close(fd);
		}
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{	
	switch (pollData._pollState)
	{
		case LISTENER:
			acceptNewConnection(pollFd.fd);
			break ;
		case CLIENT_CONNECTION_READY: 
			handleRequest(pollFd.fd);
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
