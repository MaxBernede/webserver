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
		void fill_body(std::istringstream &iss);
		void fill_file();
		void parse_body(std::istringstream &iss, std::string &line);

		//Methods
		void show_datas();

		std::string get_values(std::string key);
		std::string get_method(int index);
		std::string get_html();
		std::string get_file();
		std::string get_file_name();

	private:
		std::vector<std::pair<std::string, std::string>> _request;
		std::string _boundary;
		std::vector<std::string> _method;
		std::string _file;
	
		std::string	request_str; // parsed request string
		std::string	_request_text;

		int			_clientFd;
		bool		doneReading;
};