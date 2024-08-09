#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>

#include "Server.hpp"
#include "Request.hpp"
#include "Redirect.hpp"
#include "AutoIndex.hpp"

#define BUFFER_SIZE 1024

class CGI;
class Request;

class Response {
public:
	Response(int clientFd);
	~Response();

	//Members functions
	std::string makeStrResponse(Request* request);
	void		addHeaders(Request* request);
	void 		addToBuffer(std::string buffer);
	void		setResponseString(std::string response);
	void 		setReady(void);
	void 		rSend();
	void		sendRedir();
	void		setDirectoryListing(Request* request);
	int			setRedirectStr(int status, std::string from, std::list<s_redirect> redirs);
	void		errorResponseHTML(ErrorCode);

private:
	int			_clientFd;
	std::string _file;
	std::string _responseText;
	bool		_ready;
};
