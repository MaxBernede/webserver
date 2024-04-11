#pragma once

#include <string>
#include "Server.hpp"

const static std::string methodType[8] = {
	"GET",
	"POST",
	"DELETE",
	"PUT",
	"PATCH",
	"CONNECT",
	"OPTIONS",
	"TRACE"};

class Request {
	public:
		Request();
		Request(int fd);
		Request(std::string request);

		int			getType() const;
		std::string	getHost() const;
	private:
		std::string _request;
		int			_type;
		std::string	_host;
};

std::ostream & operator<< (std::ostream &out, const Request& src);