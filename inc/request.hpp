#pragma once
#include "webserver.hpp"

enum reqType
{
	HTML,
	IMG
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
		void		checkRequest();

		//GET
		std::string	getFile();
		// std::string	getExtension(std::string fileName);
		std::string	getFileName();
		int			getClientFd();
		int			getRequestPort();
		std::string	getRequestHost();
		std::string	getRequestStr();
		std::string	getMethod(int index);
		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string getResponse(void);
		std::string getFileNameProtected( void );
		Server		getConfig();
		// SET
		void		setFile();
		void		setConfig(Server &config);	

		//Methods
		bool		isRedirect();
		bool		isCgi(); // boolean to tell if request is Cgi
		bool		isDoneReading();
		bool		isBoundary(const std::string &line);

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

};