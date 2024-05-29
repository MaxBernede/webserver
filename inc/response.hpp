#pragma once

#include "webserver.hpp"
#include "Server.hpp"
#include "request.hpp"
#include "CGI.hpp"
#include<utils.hpp>

#define BUFFER_SIZE 1024

class CGI;

class Response {
	public:
		Response(Request *req, int clientFd);
		Response(CGI *cgi, int clientFd);
		~Response();

		//Members functions
		std::string makeResponse(std::ifstream &file);
		std::string makeStrResponse( void );
		std::string readHtmlFile( void );
		std::string readFromPipe( void );
		void addToBuffer(std::string buffer);
		void setReady( void );
		void rSend( void );
	
	private:
		std::string file;
		std::string response_text;
		std::string	_html_file;
		Request	*_request;
		CGI *_cgi;
		int	_clientFd;
		int	_readFd;
		bool ready;
		const std::unordered_map<std::string,std::string> contentType = {
			{"html","text/html"},
			{"ico","image/x-icon"},
			{"cgi","text/html"}

		};
};