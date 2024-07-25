#pragma once
#include "Webserver.hpp"

class Response;

class HTTPObject {
	private:
		int			_clientFd;
		int			_readFd; // fileFd or pipeFd, only for GET requests
	
	public:
		Server 		_config;
		Request		*_request;
		Response	*_response;
		CGI			*_cgi;
		
		HTTPObject(int clientFd);
		~HTTPObject(void);
		
		std::string redirectResponse(void);
		void		sendResponse(void);
		void		createCGI();
		void		runCGI(void);
		
		// Getters & Setters
		void		setConfig(Server config);
		void		setReadFd(int fd);
		bool		isCgi(void);
		int			getClientFd(void); 
		int			getReadFd(void);
};
