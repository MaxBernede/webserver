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
	// CGI *cgiRequest = new CGI(_requests[clientFd], clientFd);
	// int pipeFd = cgiRequest->getReadFd();
	// _cgi[pipeFd] = cgiRequest;
	// cgiRequest->runCgi();
	_httpObjects[clientFd]->createCGI();
	int pipeFd = _httpObjects[clientFd]->_cgi->getReadFd();
	_httpObjects[clientFd]->setReadFd(pipeFd);
	addQueue(CGI_READ_WAITING, pipeFd);
	_httpObjects[clientFd]->_cgi->runCgi();
}

void ServerRun::handleStaticFileRequest(int clientFd)
{
	// // TODO check if _requests[clientFd]->getFileName() is defined in the configs redirect
	// std::string fileName = _requests[clientFd]->getFileName(); //! WARNING FILENAME CHANGED
	// if (fileName.empty() || fileName == "/") // this fix have been added because no possible skip for this function
	// 	fileName = "index.html"; // This needs to be changed, temporary here for the HEAD request
	// std::string filePath = _requests[clientFd]->getConfig().getRoot() + "html/" + fileName; // TODO root path based on config
	
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
	//_requests[fileFd] = _requests[clientFd]; // TODO: We need to add the request header reading in here too
	addQueue(FILE_READ_READING, fileFd);
}

// Only handles 404 and 405
void ServerRun::redirectToError(int ErrCode, int clientFd)
{
	//check this code to implement the no reply 204 case of successfull delete or stuff like that
	// if (_responses.find(clientFd) == _responses.end()) 
	// {
	// 	Response *response = new Response(request, clientFd, true);
	// 	if (ErrCode == 404)
	// 		response->setResponseString(NOT_FOUND);
	// 	if (ErrCode == 405)
	// 		response->setResponseString(NOT_ALLOWED);
	// 	if (ErrCode == NO_CONTENT)
	// 		response->setResponseString("HTTP/1.1 204 No Content");
	// 	if (ErrCode == ErrorCode::CONFLICT)
	// 		response->setResponseString(HTTP_CONFLICT_RESPONSE);
	// 	if (ErrCode == ErrorCode::FORBIDDEN)
	// 		response->setResponseString(HTTP_FORBIDDEN_RESPONSE);
	// 	_responses[clientFd] = response;
	// 	_pollData[clientFd]._pollType = SEND_REDIR;
	// }
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
		std::cout << "reading\n";
		_httpObjects[clientFd]->_request->readRequest();
	}
	if (_httpObjects[clientFd]->_request->isDoneReading() == true)
	{
		std::cout << "managing\n";
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
				std::cout << "CGI is not allowed for this server\n";
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
	// if (_requests.find(clientFd) == _requests.end())
	// {
	// 	Request *newRequest = new Request(clientFd);
	// 	_requests[clientFd] = newRequest;
	// }
	// else if (_requests[clientFd]->isDoneReading() == false)
	// {
	// 	_requests[clientFd]->readRequest();
	// }
	// if (_requests[clientFd]->isDoneReading() == true)
	// {
	// 	int port = _requests[clientFd]->getRequestPort();
	// 	if (port < 0)
	// 	{
	// 		throw Exception("Port not found", errno);
	// 		exit(1);
	// 	}
	// 	Server config = getConfig(port);
	// 	//TODO if server == not found, error should be thrown, please catch
	// 	_requests[clientFd]->setConfig(config);
	// 	int ErrCode = _requests[clientFd]->checkRequest(); // Max code : this is a request.getErrorCode();
	// 	//if (ErrCode != 0) //Yesim code
	// 	if (ErrCode != 200 && _requests[clientFd]->getErrorPageStatus() == false)
	// 	{
	// 		_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
	// 		redirectToError(ErrCode, _requests[clientFd], clientFd);
	// 		return ;
	// 	}
	// 	_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
	// 	if (_requests[clientFd]->isCgi()) // What do we do when CGI is not allowed?
	// 	{
	// 		if (!config.getCGI())
	// 		{
	// 			std::cout << "CGI is not allowed for this server\n";
	// 			return ;
	// 		}
	// 		handleCGIRequest(clientFd);
	// 	}
	// 	else if (_requests[clientFd]->getMethod(0) == "HEAD") // or anything that doesnt need READ file
	// 	{
	// 		Response *response = new Response(_requests[clientFd], clientFd, false);
	// 		_responses[clientFd] = response;
	// 		_pollData[clientFd]._pollType = FILE_READ_DONE;
	// 		return ;
	// 	}
	// 	else // Static file
	// 	{
	// 		handleStaticFileRequest(clientFd);
	// 	}
// 	}
}

void ServerRun::readFile(int fd) // Static file fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	// int clientFd = _requests[fd]->getClientFd();
	// if (_responses.find(clientFd) == _responses.end()) // Response object not created
	// {
	// 	Response *response = new Response(_requests[fd], clientFd, false);
	// 	_responses[clientFd] = response;
	// }
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
	// if (_cgi[fd]->waitCgiChild())
	// {
		char buffer[BUFFER_SIZE];

		memset(buffer, '\0', BUFFER_SIZE);
		HTTPObject *obj = findHTTPObject(fd);
		// int clientFd = _cgi[fd]->getClientFd();
		// if (_responses.find(clientFd) == _responses.end()) // Response object not created
		// {
		// 	Response *response = new Response(_cgi[fd]->getRequest(), clientFd, false);
		// 	_responses[clientFd] = response;
		// }
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
	// }
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{	
	std::cout << "Data in..." << std::endl;
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
