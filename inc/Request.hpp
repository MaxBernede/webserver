#pragma once
#include "Webserver.hpp"
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
    CODE_NOT_FOUND = 404,
    CONFLICT = 409,
	INTERNAL_SRV_ERR = 500
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
		~Request();
		Request(const Request &src);
		Request &operator=(const Request &src);
		void	constructRequest();

		void		fillBoundary(std::string text);
		std::string	getValues(std::string key);
		void		readRequest();
		void		parseFirstLine(std::istringstream &iss);
		void		parseBody(std::istringstream &iss, std::string &line);
		void 		parseResponse(const std::string& headers);
		int			checkRequest();

		//GET
		std::string	getFile();
		std::string	getFileName();
		int			getClientFd();
		int			getRequestPort();
		s_domain	getRequestDomain();
		std::string	getRequestStr();
		std::string	getMethod(int index);
		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string getResponse(void);
		std::string getFileNameProtected( void );
		Server		getConfig();
		std::string	getDeleteFilename(const std::string& httpRequest);
		ErrorCode	getErrorCode();
		std::string	getErrorString();
		// SET
		void		setFileName(std::string newName);
		void		setFile();
		void		setConfig(Server &config);	

		//Methods
		bool		isRedirect();
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

		// reconfigs the config object to location specifics, if necessary
		void		configConfig();

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
		uint32_t	_read_bytes;
		Server		_config;
		ErrorCode	_errorCode;

};