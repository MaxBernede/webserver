#pragma once

#include "Request.hpp"
#include <iostream>
#include <vector>
#include <signal.h>
#include <sys/wait.h>

#define ENV_SIZE 18

class Request;
/*
I need the following from the client request parser:
- file name
- file path
- request header data
- method type
- script name (aka same as file path for example, var/www/python.cgi )
- port (if possible)
- server name (if possible)
- remove address / host (?) not sure what these are
*/

class CGI {
	private:
		int							_pid;
		int							_responsePipe[2]; // pipe where CGI sends data
		int							_uploadPipe[2]; // post data to CGI child process
		Request						*_request;
		int							_clientFd;
		std::vector<std::string>	_cgiEnvArr;
		char*const					*_cgiEnvCStr;
		std::chrono::time_point<std::chrono::steady_clock> _forkTime;

	public:
		CGI(Request *request, int clientFd);
		~CGI(void);

		// Cgi methods
		void	run();
		void	makeEnvCStr();
		void	makeEnvArr();
		bool 	waitCgiChild();
		bool	isTimeOut();
		void	killChild();
		void	closeUploadPipe();
		// Getters 
		int		getReadFd();
		int		getWriteFd();
		int		getClientFd();
		Request	*getRequest();

};

/*
ENV VARIABLES
Key 	Value
DOCUMENT_ROOT 	The root directory of your server
HTTP_COOKIE 	The visitor's cookie, if one is set
HTTP_HOST 	The hostname of the page being attempted
HTTP_REFERER 	The URL of the page that called your program
HTTP_USER_AGENT 	The browser type of the visitor
HTTPS 	"on" if the program is being called through a secure server
PATH 	The system path your server is running under
QUERY_STRING 	The query string (see GET, below)
REMOTE_ADDR 	The IP address of the visitor
REMOTE_HOST 	The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)
REMOTE_PORT 	The port the visitor is connected to on the web server
REMOTE_USER 	The visitor's username (for .htaccess-protected pages)
REQUEST_METHOD 	GET or POST
REQUEST_URI 	The interpreted pathname of the requested document or CGI (relative to the document root)
SCRIPT_FILENAME 	The full pathname of the current CGI
SCRIPT_NAME 	The interpreted pathname of the current CGI (relative to the document root)
SERVER_ADMIN 	The email address for your server's webmaster
SERVER_NAME 	Your server's fully qualified domain name (e.g. www.cgi101.com)
SERVER_PORT 	The port number your server is listening on
SERVER_SOFTWARE 	The server software you're using (e.g. Apache 1.3) 
*/
