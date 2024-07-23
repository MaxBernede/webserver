#include "webserver.hpp"

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

void ServerRun::acceptNewConnection(int listenerFd)
{
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	if (connFd == -1)
		throw(Exception("Error: accept() failed and returned -1", errno));
	Logger::log("New client connection accepted at fd: " + std::to_string(connFd), LogLevel::INFO);
	addQueue(CLIENT_CONNECTION_READY, connFd);
}

void ServerRun::handleCGIRequest(int clientFd)
{
	std::cout << "CGI Request\n";
	Logger::log("A CGI Request is being handled", LogLevel::INFO);
	_httpObjects[clientFd]->createCGI();
	int pipeFd = _httpObjects[clientFd]->_cgi->getReadFd();
	_httpObjects[clientFd]->setReadFd(pipeFd);
	addQueue(CGI_READ_WAITING, pipeFd);
	_httpObjects[clientFd]->_cgi->runCgi();
}

void ServerRun::handleStaticFileRequest(int clientFd)
{
	std::string filePath = _httpObjects[clientFd]->_request->getFilePath();
	std::cout << "Opening static file: " << filePath << std::endl;
	int fileFd = open(filePath.c_str(), O_RDONLY);
	if (fileFd < 0)
	{
		std::cout << "Failed opening file: " << filePath << std::endl; // TODO 404 error
		throw(Exception("Opening static file failed", errno));
	}
	Logger::log("File correctly opened", INFO);
	_httpObjects[clientFd]->setReadFd(fileFd);
	addQueue(FILE_READ_READING, fileFd);
}

// Only handles 404 and 405
void ServerRun::redirectToError(int ErrCode, int clientFd)
{
	HTTPObject *obj = _httpObjects[clientFd];
	if (ErrCode == 404)
		obj->_response->setResponseString(NOT_FOUND);
	if (ErrCode == 405)
		obj->_response->setResponseString(NOT_ALLOWED);
	if (ErrCode == NO_CONTENT)
		obj->_response->setResponseString("HTTP/1.1 204 No Content");
	if (ErrCode == ErrorCode::CONFLICT)
		obj->_response->setResponseString(HTTP_CONFLICT_RESPONSE);
	if (ErrCode == ErrorCode::FORBIDDEN)
		obj->_response->setResponseString(HTTP_FORBIDDEN_RESPONSE);
	_pollData[clientFd]._pollType = SEND_REDIR;
}

// Only continue after reading the whole request
void ServerRun::readRequest(int clientFd)
{
	if (_httpObjects.find(clientFd) == _httpObjects.end())
	{
		HTTPObject *newObj = new HTTPObject(clientFd);
		_httpObjects[clientFd] = newObj;
	}
	if (_httpObjects[clientFd]->_request->isDoneReading() == false)
	{
		_httpObjects[clientFd]->_request->readRequest();
	}
	if (_httpObjects[clientFd]->_request->isDoneReading() == true)
	{
		Server config = getConfig(clientFd);
		_httpObjects[clientFd]->setConfig(config);
		int ErrCode = _httpObjects[clientFd]->_request->checkRequest(); 
		if (ErrCode != 200 && _httpObjects[clientFd]->_request->getErrorPageStatus() == false)
		{
			_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
			redirectToError(ErrCode, clientFd);
			return ;
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
		if (_httpObjects[clientFd]->isCgi()) // What do we do when CGI is not allowed?
		{
			if (!config.getCGI())
			{
				throw(Exception("CGI is not permitted for this server", 1));
				return ;
			}
			handleCGIRequest(clientFd);
		}
		else if (_httpObjects[clientFd]->_request->getMethod(0) == "HEAD") // or anything that doesnt need READ file
		{
			_pollData[clientFd]._pollType = FILE_READ_DONE;
			return ;
		}
		else // Static file
		{
			handleStaticFileRequest(clientFd);
		}
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
		_pollData[fd]._pollType = FILE_READ_DONE;
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
			_pollData[fd]._pollType = CGI_READ_DONE;
			obj->_response->setReady();
			close(fd);
		}
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{	
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
