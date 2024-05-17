#pragma once
#include "webserver.hpp"

//TODO getter for port and host
class Request
{
	public:
		Request(int clientFd);
		~Request();

		void		fill_boundary(std::string text);
		std::string	get_values(std::string key);
		void		readRequest();
		std::string	getFileName();
		int			getClientFd();
		int			getRequestPort();
		bool		isCgi(); // boolean to tell if request is Cgi
		bool		isDoneReading();
		std::vector<std::pair<std::string, std::string>> parse_response(const std::string& headers);

		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string		getRequestStr();

	private:
		std::vector<std::pair<std::string, std::string>>	_request;
		std::string	request_str; // parsed request string
		std::string	_boundary;
		int			_clientFd;
		std::string	_request_text;
		bool		doneReading;
};