#pragma once
#include "webserver.hpp"
#include "server.hpp"
#include "request.hpp"
#include "CGI.hpp"

#define BUFFER_SIZE 1024

class CGI;

class Response {
	public:
		Response(Request *req, int clientFd, bool def_error);
		~Response();

		//Members functions
		std::string makeStrResponse( void );
		void addToBuffer(std::string buffer);
		void setResponseString(std::string response);
		void setReady( void );
		void rSend( void );
		
		std::string redirectResponse( void );
	
	private:
		std::string _file;
		std::string _response_text;
		std::string	_html_file;
		Request	*_request;
		int	_clientFd;
		bool _ready;
		bool _default_error;
		const std::unordered_map<std::string,std::string> contentType = {
			{"html","text/html"},
			{"ico","image/x-icon"},
			{"cgi","text/html"}

		};
};
