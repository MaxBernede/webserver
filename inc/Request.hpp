#pragma once
#include "Webserver.hpp"
#include <cstdio>


#include <filesystem>
#include <vector>

#include "Exception.hpp"
#include "Logger.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>

#define MAX_BODY_SIZE 1048576
//#define MAX_BODY_SIZE 1048

enum reqType
{
	HTML,
	IMG
};

enum methodField {
	METHOD,
	LINK,
	HTML_TYPE,
};

#define METHODS {"GET",	"POST",	"DELETE", "PUT", "PATCH", "CONNECT", "OPTIONS", "TRACE"}

class RequestException : public std::exception {
private:
    std::string _message;
	LogLevel _lvl;

public:
    // Constructor that takes a message
	explicit RequestException(const std::string& message) : _message(message){
		_lvl = LogLevel::INFO;
	}
    explicit RequestException(const std::string& message, LogLevel lvl) : _message(message), _lvl(lvl){}

    // Override the what() method to provide the error message
    const char* what() const noexcept override {
		Logger::log("[EXCEP] " + _message, _lvl);
        return _message.c_str();
    }
};

//TODO getter for port and host
class Request
{
	public:
		//constructors
		Request(int clientFd);
		Request(void);
		~Request();
		void		constructRequest();

		void		fillBoundary(std::string text);
		std::string	getValues(std::string key);
		void		readRequest();
		void		parseFirstLine(std::istringstream &iss);
		void		parseBody(std::istringstream &iss, std::string &line);
		void 		parseRequest(const std::string& headers);
		void		checkRequest();

		//GET
		std::string	getFile();
		std::string	getFileName();
		std::string	getServerName();
		int			getClientFd();
		int			getRequestPort();
		s_domain	getRequestDomain(); //dup of getrequesthost
		std::string	getRequestHost();
		std::string	getRequestStr();
		std::string	getMethod(size_t index);
		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string getFileNameProtected( void );
		Server		getConfig();
		std::string	getDeleteFilename(const std::string& httpRequest);
		ErrorCode	getErrorCode();
		std::string	getErrorString();
		std::string	getFilePath();
		bool		getErrorPageStatus();
		bool		isEmptyResponse();
		void		execAction();
		std::string	getBoundary();
		std::string	getBody();

		// SET
		void		setFileName(std::string newName);
		void		setFile();
		void		setConfig(Server config);
		void		setErrorCode(ErrorCode);

		//Methods
		void		handleRedirection();
		bool		isCgi(); // boolean to tell if request is Cgi
		bool		isDoneReading();
		bool		isBoundary(const std::string &line);

		void		redirRequest501();
		void		redirRequest405();
		void		redirRequest404();
		void		handleDirListing();
		void		requestReadTooLong();

		void		searchErrorPage();
		void		handleDelete();
		void		handlePost();
		void		handleDirDelete(std::string & path);
		
		void		remove(std::string & path);
		void		removeDir(std::string & path);

		void		checkErrors();
		void		checkVersion();
		void		tooLong();

		void		configConfig();

		void		printAllData();
		void		startConstruRequest();

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
		std::string	_filePath;
		bool		_errorPageFound;
};
