#include "ServerRun.hpp"

void ServerRun::sendResponse(int fd) // Using readFd
{
		Logger::log("Sending response to fd: " + std::to_string(fd), WARNING);
		HTTPObject *obj = findHTTPObject(fd);
		obj->sendResponse();
		removeConnection(fd);
		int clientFd = obj->getClientFd();
		cleanUp(clientFd);
}

void ServerRun::sendError(int clientFd)
{
	Logger::log("Sending Redir msg", INFO);
	_httpObjects[clientFd]->sendResponse();
	cleanUp(clientFd);
}

void ServerRun::uploadToCgi(int writePipe)
{
	HTTPObject *obj = findHTTPObject(writePipe);
	obj->writeToCgiPipe();
	close(writePipe); // close write side of the pipe
	obj->_cgi->closeUploadPipe();
	removeConnection(writePipe);
	_pollData[writePipe]._pollState = CGI_WRITE_STOP;
}

// Sending data from the server to the client
void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd)
{
	// std::cout << "PollState: " << pollData._pollState << std::endl;
	// exit(1);
	switch (pollData._pollState)
	{
		case CGI_READ_DONE:
			sendResponse(pollFd.fd);
			break ;
		case FILE_READ_DONE:
			sendResponse(pollFd.fd); // combine cgi_read_done & file_read_done
			break ;
		case EMPTY_RESPONSE:
			sendError(pollFd.fd);
			break;
		case HTTP_ERROR:
			sendError(pollFd.fd);
			break ;
		case HTTP_REDIRECT:
			sendRedirect(pollFd.fd);
			break ;
		case CGI_WRITE_TO_PIPE:
			uploadToCgi(pollFd.fd);
			break ;
		default:
			break ;
	}
}

void ServerRun::sendRedirect(int clientFd) // this is a clientFd!
{
	_httpObjects[clientFd]->sendRedirection();
	cleanUp(clientFd);
}

void ServerRun::cleanUp(int clientFd)
{
	close(clientFd);
	removeConnection(clientFd);
	if (_httpObjects.count(clientFd))
	{
		delete _httpObjects[clientFd];
		_httpObjects.erase(clientFd);
	}
}
