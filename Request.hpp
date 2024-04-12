#pragma once

#include <string>
#include<list>
// #include "Server.hpp"
enum methods{
	GET,
	POST,
	DELETE,
	PUT,
	PATCH,
	CONNECT,
	OPTIONS,
	TRACE
};

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
		std::string	getContType() const;
		uint32_t	getSize() const;
		std::string	getContent() const;
	private:
		std::list<std::string>	_request; //the entirety of the request, as a list of strings
		int						_method;
		std::string				_host;
		std::string				_contentType; // what content are we trying to POST?
		uint32_t				_size;	//for POST requests, cannot be higher then max body
		std::string				_content; // the content of the POST request string? maybe something else?
};

std::ostream & operator<< (std::ostream &out, const Request& src);
