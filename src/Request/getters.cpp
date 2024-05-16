#include "webserver.hpp"
#include "request.hpp"

//0 : GET/POST/ETC
//1 : LINK
//2 : HTTP Version
std::string request::get_method(int index){
	if (index >= _method.size())
		return "";
	return _method[index];
}

//Return the value of the found key, otherwise empty string
std::string request::get_values(std::string key){
	for (const auto& pair : _request) {
		if (pair.first == key)
			return pair.second;
	}
	return "";
}

std::string request::get_file(){
	return _file;
}

//Return the first word after GET (usually the html) otherwise empty
std::string request::get_html(){
	std::string val = get_values("GET");
	if (val.empty()){
		val = get_values("POST");
		if (val.empty())
			return "";
	}
	std::string html_file = firstWord(val);
	if (html_file == "/")
		return "index.html";
	return html_file;
}
