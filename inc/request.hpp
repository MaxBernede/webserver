#pragma once
#include "webserver.hpp"

class request
{
	public:
		//Constructor
		request(std::string);

		//Parsing
		void parse_first_line(std::istringstream &iss);
		void fill_boundary(std::string text);
		bool is_boundary(const std::string &line);
		void parse_response(const std::string& headers);

		//Methods
		void show_datas();

		std::string get_values(std::string key);
		std::string get_html();
	private:
		std::vector<std::pair<std::string, std::string>> _request;
		std::string _boundary;
		std::vector<std::string> _method;
};