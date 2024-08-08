#include "ServerRun.hpp"

void ServerRun::acceptNewConnection(int listenerFd)
{
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	if (connFd == -1 && !(errno == EAGAIN || errno == EWOULDBLOCK))
		throw (Exception("Error: accept() failed and returned -1", errno));
	Logger::log("New client connection accepted at fd: " + std::to_string(connFd), LogLevel::DEBUG);
	addQueue(CLIENT_CONNECTION_READY, CLIENTFD, connFd);
}

void ServerRun::handleCgiRequest(int clientFd)
{
	Logger::log("A CGI Request is being handled", LogLevel::INFO);
	_httpObjects[clientFd]->createCgi();
	int readFd = _httpObjects[clientFd]->_cgi->getReadFd();
	int writeFd = _httpObjects[clientFd]->_cgi->getWriteFd();
	_httpObjects[clientFd]->setReadFd(readFd);
	_httpObjects[clientFd]->setWriteFd(writeFd);
	addQueue(CGI_READ_WAITING, READFD, readFd);
	addQueue(CGI_WRITE_TO_PIPE, WRITEFD, writeFd);
	_httpObjects[clientFd]->runCgi();
}

void ServerRun::handleStaticFileRequest(int clientFd)
{
	std::string filePath = _httpObjects[clientFd]->_request->getFilePath();
	Logger::log("Opening static file: " + filePath, LogLevel::INFO);
	int fileFd = open(filePath.c_str(), O_RDONLY);
	if (fileFd < 0)
	{
		Logger::log("Failed opening file: " + filePath, LogLevel::ERROR);
		throw (HTTPError(ErrorCode::PAGE_NOT_FOUND));
	}
	Logger::log("File successfully opened", INFO);
	_httpObjects[clientFd]->setReadFd(fileFd);
	addQueue(FILE_READ_READING, READFD, fileFd);
}

void ServerRun::DirectoryListing(int clientFd){
	Logger::log("Auto index on.", LogLevel::DEBUG);
	HTTPObject *obj = _httpObjects[clientFd];
	obj->_response->setDirectoryListing(_httpObjects[clientFd]->_request);
}

// Handles error code when no error file exists
void ServerRun::redirectToError(ErrorCode ErrCode, int Fd)
{
	Logger::log("Redirecting to Error...", LogLevel::WARNING);

	HTTPObject *obj;
	if (_pollData[Fd]._fdType == CLIENTFD)
		obj = _httpObjects[Fd];
	else
		obj = findHTTPObject(Fd);
	obj->_request->searchErrorPage();
	// int c = obj->_request->getErrorCode(); < not sure what this is for?
	if (ErrCode == HTTP_NOT_SUPPORT)
		_pollData[obj->getClientFd()]._pollState = HTTP_ERROR;
	else if (obj->_request->getErrorPageStatus() == false) // if no error file does not exst
	{
		Logger::log("Error page does not exist..Error code: " + std::to_string(ErrCode), LogLevel::WARNING);
		obj->_response->errorResponseHTML(ErrCode);
		_pollData[obj->getClientFd()]._pollState = HTTP_ERROR;
	}
	else // if error page exists
		handleStaticFileRequest(obj->getClientFd());
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
		Logger::log("Creating a new HTTPObject", LogLevel::INFO);
		HTTPObject *newObj = new HTTPObject(clientFd);
		_httpObjects[clientFd] = newObj;
	}
	if (_httpObjects[clientFd]->_request->isDoneReading() == false)
		_httpObjects[clientFd]->_request->readRequest();
	if (_httpObjects[clientFd]->_request->isDoneReading() == true)
	{
		_httpObjects[clientFd]->_request->printAllData();
		_httpObjects[clientFd]->_request->startConstruRequest();
		s_domain Domain = _httpObjects[clientFd]->_request->getRequestDomain();
		Server config = findConfig(Domain);
		_httpObjects[clientFd]->setConfig(config);
		_httpObjects[clientFd]->_request->checkRequest();
		_pollData[clientFd]._pollState = CLIENT_CONNECTION_WAIT;
		executeRequest(clientFd);
	}
}

void ServerRun::executeRequest(int clientFd){
	if (_httpObjects[clientFd]->isCgi()) // GET and POST for CGI
	{
		Logger::log("CGI Request received...", LogLevel::INFO);
		if (!_httpObjects[clientFd]->_config.getCGI())
		{
			Logger::log("CGI is not permitted for this server", LogLevel::ERROR);
			throw (HTTPError(ErrorCode::FORBIDDEN));
		}
		handleCgiRequest(clientFd);
	}
	else if (_httpObjects[clientFd]->_request->isEmptyResponse()) { // POST or DEL or HEAD
		
		_pollData[clientFd]._pollState = EMPTY_RESPONSE;
		_httpObjects[clientFd]->_request->execAction();
	}
	else // Static file
		handleStaticFileRequest(clientFd);
}

void ServerRun::readFile(int fd) // Static file fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	HTTPObject *obj = findHTTPObject(fd);
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(HTTPError(ErrorCode::PAGE_NOT_FOUND));
	if (readChars > 0)
		obj->_response->addToBuffer(std::string(buffer, readChars));
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
	Logger::log("Reading the pipe read end...", LogLevel::DEBUG);
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read pipe failed!", 1));
	if (readChars > 0)
		obj->_response->addToBuffer(std::string(buffer, readChars));
	if (readChars < BUFFER_SIZE - 1)
	{
		_pollData[fd]._pollState = CGI_READ_DONE;
		obj->_response->setReady();
		close(fd);
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
