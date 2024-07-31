#pragma once

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "CGI.hpp"
#include <chrono>

class Response;

class HTTPObject {
	private:
		int			_clientFd;
		int			_readFd; // fileFd or pipeFd, only for GET requests
		int			_writeFd; // upload pipe read end to write to cgi child process
		bool 		_doneWritingToCgi;
		std::chrono::time_point<std::chrono::high_resolution_clock>	_startTime;
	
	public:
		Server 		_config;
		Request		*_request;
		Response	*_response;
		CGI			*_cgi;
		
		HTTPObject(int clientFd);
		~HTTPObject();
		
		void	sendRedirection();
		void	sendResponse();
		void	writeToCgiPipe();
		void	createCgi();
		void	runCgi();
		bool	isCgi();
		void	checkTimeOut();
		
		// Getters & Setters
		void	setConfig(Server config);
		void	setReadFd(int fd);
		void	setWriteFd(int fd);
		int		getClientFd(); 
		int		getReadFd();
		int		getWriteFd();
		std::chrono::time_point<std::chrono::high_resolution_clock> getStartTime();
};
