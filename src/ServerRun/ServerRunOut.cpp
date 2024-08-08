#include "ServerRun.hpp"

void ServerRun::sendResponse(int readFd) // Using readFd
{
	Logger::log("Sending response", INFO);
	HTTPObject* obj = findHTTPObject(readFd);
	obj->sendResponseWithHeaders();
	removeConnection(readFd);
	int clientFd = obj->getClientFd();
	cleanUp(clientFd);
}

// Cgi response does not add header as this is done in the CGI script
void ServerRun::sendCgiResponse(int pipeFd) // Using pipeFd read side
{
	Logger::log("Sending response from CGI", INFO);
	HTTPObject* obj = findHTTPObject(pipeFd);
	obj->_response->rSend();
	removeConnection(pipeFd);
	int clientFd = obj->getClientFd();
	cleanUp(clientFd);
}

void ServerRun::sendError(int clientFd)
{
	Logger::log("Sending error page", INFO);
	_httpObjects[clientFd]->sendResponseWithHeaders();
	cleanUp(clientFd);
}

void ServerRun::uploadToCgi(int writePipe)
{
	HTTPObject* obj = findHTTPObject(writePipe);
	obj->writeToCgiPipe();
	if (obj->getWriteFinished())
	{
		obj->_cgi->closeUploadPipe();
		removeConnection(writePipe);
	}
}

void ServerRun::sendRedirect(int clientFd) // this is a clientFd
{
	Logger::log("Sending redirection page", INFO);
	_httpObjects[clientFd]->_response->rSend();
	cleanUp(clientFd);
}

// Sending data from the server to the client
void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd)
{
	switch (pollData._pollState)
	{
	case CGI_READ_DONE:
		sendCgiResponse(pollFd.fd);
		break;
	case FILE_READ_DONE:
		sendResponse(pollFd.fd);
		break;
	case EMPTY_RESPONSE:
		sendError(pollFd.fd);
		break;
	case HTTP_ERROR:
		sendError(pollFd.fd);
		break;
	case HTTP_REDIRECT:
		sendRedirect(pollFd.fd);
		break;
	case CGI_WRITE_TO_PIPE:
		uploadToCgi(pollFd.fd); // write to cgi pipe
		break;
	case AUTO_INDEX:
		sendAutoIndex(pollFd.fd);
	default:
		break;
	}
}

void ServerRun::sendAutoIndex(int clientFd)
{
	_httpObjects[clientFd]->sendAutoIndex();
	cleanUp(clientFd);
}


void ServerRun::cleanUp(int clientFd)
{
	Logger::log("Cleaning up client connection", INFO);
	close(clientFd);
	removeConnection(clientFd);
	if (_httpObjects.count(clientFd))
	{
		delete _httpObjects[clientFd];
		_httpObjects.erase(clientFd);
	}
}
