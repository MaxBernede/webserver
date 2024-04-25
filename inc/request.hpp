#pragma once
#include "webserver.hpp"

class request
{
	public:
		request(std::string);
		void fill_boundary(std::string text);
		bool is_boundary(const std::string &line);
		void parse_response(const std::string& headers);
		std::string get_values(std::string key);
		std::string get_html();
	private:
		std::vector<std::pair<std::string, std::string>> _request;
		std::string _boundary;
};