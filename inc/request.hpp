#pragma once
#include "webserver.hpp"

class Request
{
	public:
		Request(int clientFd);
		~Request();

		void		fill_boundary(std::string text);
		std::string	get_values(std::string key);
		std::string	getFileName();
		int			getClientFd();
		bool		isCgi(); // boolean to tell if request is Cgi

		std::vector<std::pair<std::string, std::string>>	getContent();
		std::string		getRequestStr();

	private:
		std::vector<std::pair<std::string, std::string>>	_request;
		std::string	request_str; // parsed request string
		std::string	_boundary;
		int			_clientFd;
};