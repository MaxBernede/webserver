#pragma once
#include "webserver.hpp"

enum reqType
{
	HTML,
	IMG
};

//TODO getter for port and host
class Request
{
	public:
		//constructors
		Request(int clientFd);
		~Request();
		void	constructRequest();

		//SET
		void		setFile();
		void		fillBoundary(std::string text);
		std::string	getValues(std::string key);
		void		readRequest();
		void		parseFirstLine(std::istringstream &iss);
		void		parseBody(std::istringstream &iss, std::string &line);
		void 		parseResponse(const std::string& headers);
		std::string	getExtension(std::string fileName);

		//GET
		std::string	getFile();
		std::string	getFileName();
		int			getClientFd();
		int			getRequestPort();
		std::string	getRequestHost();
		std::string	getRequestStr();
		std::string	getMethod(int index);
		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string getResponse(void);

		// SET
		void	setConfig(Server config);	

		//Methods
		bool		isCgi(); // boolean to tell if request is Cgi
		bool		isDoneReading();
		bool		isBoundary(const std::string &line);

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
		Server		_config;
};