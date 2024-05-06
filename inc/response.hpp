#pragma once

#include "webserver.hpp"
#include "request.hpp"

#define BUFFER_SIZE 1024

class Response {
	public:
		Response(Request req, int clientFd);
		~Response();

		//Members functions
		std::string makeResponse(std::ifstream &file);
		std::string readHtmlFile(void);
		std::string readFromPipe( void );
		void readContents(void);
		void rSend();
	
	private:
		std::string file;
		std::string response_text;
		std::string	_html_file;
		Request	_request;
		int	_clientFd;
		int	_readFd;
};