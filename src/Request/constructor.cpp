#include "webserver.hpp"
#include "request.hpp"

request::request(std::string text){
	fill_boundary(text);
	parse_response(text);
	fill_file();
	show_datas();
	std::string body = get_values("Body");
	if (!body.empty())
		create_file(body, "saved_files");
}

