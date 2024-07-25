#pragma once
#include "Webserver.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Redirect.hpp"
#include "CGI.hpp"

#define BUFFER_SIZE 1024

class CGI;

class Response {
	public:
		Response(int clientFd);
		~Response();

		//Members functions
		std::string makeStrResponse(Request *request);
		void addToBuffer(std::string buffer);
		void setResponseString(std::string response);
		void setReady( void );
		void rSend(Request *request);
		std::string redirectResponse(int clientFd, std::string from, std::list<s_redirect> redirs);
	 
	private:
		int			_clientFd;
		std::string _file;
		std::string _response_text;
		bool 		_ready;
};
