#pragma once

#include "webserver.hpp"
#include "request.hpp"

class Response {
	public:
		Response(request req, int clientFd);

		//Members functions
		std::string parse_buffer(std::string buffer);
		void handle_request();
		void r_send();
	
	private:
		std::string file;
		std::string response_text;
		std::string html_file;
		request 	_request;
		int			_clientFd;
};