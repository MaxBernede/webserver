#pragma once

#include <sys/socket.h>

#include "Server.hpp"
#include "Request.hpp"
#include "Redirect.hpp"

#define BUFFER_SIZE 1024

class CGI;
class Request;

class Response {
	public:
		Response(int clientFd);
		~Response();

		//Members functions
		std::string makeStrResponse(Request *request);
		void 		addToBuffer(std::string buffer);
		void		setResponseString(std::string response);
		void 		setReady( void );
		void 		rSend(Request *request);
		void		sendRedir();
		int			setRedirectStr(int status, std::string from, std::list<s_redirect> redirs);

		void		errorResponseHTML(ErrorCode);
	 
	private:
		int			_clientFd;
		std::string _file;
		std::string _response_text;
		bool 		_ready;
};
