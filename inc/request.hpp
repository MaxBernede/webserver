#pragma once
#include "webserver.hpp"

class request
{
	public:
		request(int clientFd);
		void fill_boundary(std::string text);
		std::string get_values(std::string key);
		std::string get_html();
		int getClientFd();
		bool isCgi(); // boolean to tell if request is Cgi

		std::vector<std::pair<std::string, std::string>> getContent();
		std::string getRequestStr();

	private:
		std::vector<std::pair<std::string, std::string>> _request;
		std::string request_str;
		std::string _boundary;
		int _clientFd;
};