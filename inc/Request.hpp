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
#include <sstream>
#include <string>

#define MAX_BODY_SIZE 2147483648
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
	explicit RequestException(const std::string& message) : _message(message) {
		_lvl = LogLevel::INFO;
	}
	explicit RequestException(const std::string& message, LogLevel lvl) : _message(message), _lvl(lvl) {}

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
	~Request();
	void		constructRequest();
	void		addNecessaryHeaders();

	void		fillBoundary(std::string text);
	std::string	getValues(std::string key);
	void		readRequest(std::list<Server> _servers);
	void		parseFirstLine(std::istringstream& iss);
	void 		parseRequest(const std::string& headers);
	void		checkRequest();
	bool		finishedReadingHeader();
	void		checkHeaders(std::list<Server> _servers);
	Server		findConfig(s_domain port, std::list<Server> _servers);

	//GET
	std::string	getFileName();
	std::string	getServerName();
	int			getClientFd();
	int			getRequestPort();
	s_domain	getRequestDomain(std::string host_val);
	std::string	getRequestHost();
	std::string	getRequestStr();
	std::string	getMethod(size_t index);
	std::vector<std::pair<std::string, std::string>>	getContent();
	std::string	getFileNameProtected(void);
	Server		getConfig();
	std::string	getDeleteFilename(const std::string& httpRequest);
	std::string getOtherFilename();
	ErrorCode	getErrorCode();
	std::string	getErrorString();
	std::string	getFilePath();
	bool		getErrorPageStatus();
	bool		isEmptyResponse();
	void		execAction();
	std::string	getBoundary();
	std::string	getRawBody(); // for CGI Pipe
	bool		checkBoundary();

	// SET
	void		setFileName(std::string newName);
	void		setFile();
	void		setErrorCode(ErrorCode);

	//Methods
	void		handleRedirection();
	bool		isCgi(); // boolean to tell if request is Cgi
	bool		isDoneReading();
	bool		isBoundary(const std::string& line);

	void		redirRequest501();
	void		redirRequest405();
	void		redirRequest404();
	void		handleDirListing();

	void		searchErrorPage();
	void		handleDelete(std::string, std::string);
	void		handlePost(const std::string&, const std::string&);
	void		handleDirDelete(std::string path);
	std::string	getEndPath();
	bool		methodAccepted(std::string method);
	std::string	findFileName(std::string method);
	void		createFile(std::string const& content, std::string path, std::string file);

	void		remove(std::string path);
	void		removeDir(std::string path);

	void		checkErrors();
	void		checkVersion();

	void		configConfig();

	void		printAllData();
	
	void		printHeaders();

private:
	std::vector<std::string>	_method;
	std::vector<std::pair<std::string, std::string>>	_request;
	std::string	_boundary;
	int			_clientFd;
	std::string	_requestText;
	bool		_doneReading;
	std::string	_file;
	Server		_config;
	ErrorCode	_errorCode;
	std::string	_filePath;
	bool		_errorPageFound;
	size_t		_contentLength;
};
