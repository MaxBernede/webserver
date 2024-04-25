#pragma once

#include <iostream>

class CGI {
	private:
		int _receivePipe[2]; // pipe where CGI receives data
		int _sendPipe[2]; // pipe where CGI sends data
		std::string _response;

	public:
		CGI(void);
		~CGI(void);


};