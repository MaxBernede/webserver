#pragma once
#include "webserver.hpp"
#include "server.hpp"
#include "request.hpp"
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
		const std::unordered_map<std::string,std::string> contentType = {
			{"html","text/html"},
			{"ico","image/x-icon"},
			{"cgi","text/html"}

		};
};
