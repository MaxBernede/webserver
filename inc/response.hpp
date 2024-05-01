#pragma once

#include "webserver.hpp"

class Response {
	public:
		Response(request req, int clientFd);
		Response(int cgiFd);
		~Response();

		//Members functions
		std::string parse_buffer(std::string buffer);
		void read_contents();
		void r_send();
	
	private:
		std::string file;
		std::string response_text;
		std::string html_file;
		request 	_request;
		int			_fd;
};