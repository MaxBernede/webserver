#pragma once

#include "webserver.hpp"
#include "server.hpp"
#include "request.hpp"
#include "CGI.hpp"

#define BUFFER_SIZE 1024

class CGI;

enum class StatusCode
{
	Null = 0,
	OK = 200,
	NotFound = 404,
	UnsupportedMediaType = 415
};


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
		void setStatusCode(StatusCode code);
		void printStatusCode();
		StatusCode getStatusCode( void );
	
	private:
		std::string file;
		std::string response_text;
		std::string	_html_file;
		Request	*_request;
		CGI *_cgi;
		int	_clientFd;
		int	_readFd;
		bool ready;
		StatusCode _responseStatusCode;
		const std::unordered_map<int, std::string> _statusCode = {
			{000,	"Null"},
			{200,	"OK"},
			{404,	"Not Found"},
			{415, "Unsupported Media Type"},
		};
		const std::unordered_map<std::string, std::string> _contentType = {
			{"html",	"text/html"},
			{"ico",		"image/x-icon"},
			{"cgi",		"text/html"}
		};
};