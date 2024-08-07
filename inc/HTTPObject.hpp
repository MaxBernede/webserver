#pragma once

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "CGI.hpp"
#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

class Response;

class HTTPObject {
private:
	int			_clientFd;
	int			_readFd; // fileFd or pipeFd, only for get requests
	int			_writeFd; // upload pipe write-end to write to child process running cgi script
	TimePoint	_startTime;
	bool		_timeOut;
	size_t		_totalBytesWritten; // to child process
	bool		_writeFinished;

public:
	Server 		_config;
	Request* _request;
	Response* _response;
	CGI* _cgi;

	HTTPObject(int clientFd);
	~HTTPObject();

	void	sendRedirection(void);
	void	sendAutoIndex();

	// changed to the second one!
	void	sendResponse(void);
	void	sendResponseWithHeaders();
	void	writeToCgiPipe();
	void	createCgi();
	void	runCgi(void);
	bool	isCgi();
	void	checkTimeOut();
	// void	sendRedirection()

	// Getters & Setters
	void		setConfig();
	void		setReadFd(int fd);
	void		setWriteFd(int fd);
	int			getClientFd();
	int			getReadFd();
	int			getWriteFd();
	TimePoint	getStartTime();
	bool		getTimeOut();
	bool		getWriteFinished();
};
