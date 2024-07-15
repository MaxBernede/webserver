#pragma once
#include "webserver.hpp"
#include <cstdio>

enum reqType
{
	HTML,
	IMG
};

enum ErrorCode {
    OK = 200,
	NO_CONTENT = 204,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    CONFLICT = 409,
	INTERNAL_SRV_ERR = 500
};

#define METHODS {"GET",	"POST",	"DELETE", "PUT", "PATCH", "CONNECT", "OPTIONS", "TRACE"}

//TODO getter for port and host
class Request
{
	public:
		//constructors
		Request(int clientFd);
		~Request();
		void	constructRequest();

		void		fillBoundary(std::string text);
		std::string	getValues(std::string key);
		void		readRequest();
		void		parseFirstLine(std::istringstream &iss);
		void		parseBody(std::istringstream &iss, std::string &line);
		void 		parseResponse(const std::string& headers);
		int		checkRequest();

		//GET
		std::string	getFile();
		std::string	getFileName();
		int			getClientFd();
		int			getRequestPort();
		std::string	getRequestHost();
		std::string	getRequestStr();
		std::string	getMethod(int index);
		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string getFileNameProtected( void );
		Server		getConfig();
		std::string	getDeleteFilename(const std::string& httpRequest);
		ErrorCode	getErrorCode();
		std::string	getErrorString();

		// SET
		void		setFile();
		void		setConfig(Server config);	

		//Methods
		bool		isCgi(); // boolean to tell if request is Cgi
		bool		isDoneReading();
		bool		isBoundary(const std::string &line);
		bool		redirRequest405();
		bool		redirRequest404();
		void		handleDelete();
		void		handlePost();
		void		handleDirDelete(std::string & path);
		void		remove(std::string & path);
		void		removeDir(std::string & path);

		void		printAllData();

	private:
		std::vector<std::string> _method;
		std::vector<std::pair<std::string, std::string>>	_request;
		std::string	_request_str; // parsed request string
		std::string	_boundary;
		int			_clientFd;
		std::string	_request_text;
		bool		_doneReading;
		std::string	_file;
		uint32_t	_recv_bytes;
		Server		_config;
		ErrorCode	_errorCode;
};