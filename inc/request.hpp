#pragma once
#include "webserver.hpp"

class request
{
	public:
		request(std::string);
		void fill_boundary(std::string text);
		std::string get_values(std::string key);
		std::string get_html();
	private:
		std::vector<std::pair<std::string, std::string>> _request;
		std::string _boundary;
};