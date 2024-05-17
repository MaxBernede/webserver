#pragma once
#include "webserver.hpp"

//TODO getter for port and host
class Request
{
	public:
		//constructors
		Request(int clientFd);
		~Request();
		void construct_request();

		//SET
		void		setFile();
		void		fill_boundary(std::string text);
		std::string	get_values(std::string key);
		void		readRequest();
	
		//GET
		std::string getFile();
		std::string	getFileName();
		int			getClientFd();
		int			getRequestPort();
		std::string	getRequestStr();
		std::vector<std::pair<std::string, std::string>>	getContent();

		//Methods
		bool		isCgi(); // boolean to tell if request is Cgi
		bool		isDoneReading();
		void 		parse_response(const std::string& headers);


	private:
		std::vector<std::pair<std::string, std::string>>	_request;
		std::string	request_str; // parsed request string
		std::string	_boundary;
		int			_clientFd;
		std::string	_request_text;
		bool		doneReading;
		std::string _file;
};