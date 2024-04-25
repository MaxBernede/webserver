#include "../inc/webserver.hpp"
#include "../inc/request.hpp"

//#define found std::string::npos

//Create a pair out of the line and the int pos of the delimiter (: for every lines or space for the first line)
std::pair<std::string, std::string> create_pair(const std::string &line, size_t pos){
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1); // Skip the delimiter
	if (!value.empty() && value[0] == ' ')
		value = value.substr(1); // Remove leading space if present
	return std::make_pair(key, value);
}

//check that it's not the Content-Type defined line
//then return false or true if boundary found
bool request::is_boundary(const std::string &line){
	if (line.find("Content-Type") != std::string::npos)
		return false;
	return line.find(_boundary) != std::string::npos;
}

//save as well the GET request in the all datas AND the _method
void request::parse_first_line(std::istringstream &iss){
	std::string line, arg;
	std::getline(iss, line);
    std::istringstream line_stream(line);

    while (std::getline(line_stream, arg, ' '))
        _method.push_back(arg);

	size_t pos = line.find(' ');
	if (pos != std::string::npos)
		_request.emplace_back(create_pair(line, pos));
}

//fill the _request
// it works as : get the first line based on space
// then check for the ':' however if there is a boundary and its found, keep everything between as body
void request::parse_response(const std::string& headers) {
	std::istringstream iss(headers);
	std::string line;
	std::string body;

	body = "Body:";
	parse_first_line(iss);
	while (std::getline(iss, line)) {
		if (_boundary != "" && is_boundary(line)){
			while (std::getline(iss, line)){
				if (_boundary != "" && is_boundary(line)){
					_request.emplace_back(create_pair(body, 4));
					break;
				}
				body += line;
				body += "\n";
			}
		}
		size_t pos = line.find(':');
		if (pos != std::string::npos)
			_request.emplace_back(create_pair(line, pos));
	}
}
//return an empty string if no boundaries found
//Otherwise, return the boundary WITHOUT \r\n at the end
void request::fill_boundary(std::string text){
	std::string search = "boundary=";
	size_t pos = text.find(search);
	if (pos == std::string::npos){
		_boundary = "";
		return;
	}
	pos += search.size();
	size_t endPos = text.find("\r\n", pos);
	if (endPos == std::string::npos){
		_boundary = "";
		return;
	}
	while (text[pos] == '-')
		++pos;
    _boundary = text.substr(pos, endPos - pos);
}

void request::show_datas(){
	printColor(YELLOW, "All the datas on the Request Class :");
	printColor(RESET, "Boundary: ", _boundary);
	std::cout << " Method: ";
	for (const auto &method : _method)
		std::cout << method << " ";
	std::cout << std::endl;
	for (const auto& pair : _request)
		printColor(RESET, pair.first, ": ", pair.second);
}

//Constructor that parse everything
request::request(std::string text){
	printColor(RED, "Request constructor called");
	fill_boundary(text);
	parse_response(text);
	show_datas();
	std::string body = get_values("Body");
	if (!body.empty())
		create_file(body, "saved_files");
}

//Return the value of the found key, otherwise empty string
std::string request::get_values(std::string key){
	for (const auto& pair : _request) {
		if (pair.first == key)
			return pair.second;
	}
	printColor(RED, key, " is not found in request");
	return "";
}

//Return the first word after GET (usually the html)
//Check the scalability in case of different parsing ways to do
std::string firstWord(const std::string& str) {
	size_t pos = str.find(' ');

	if (pos == std::string::npos)
		return str;
	return str.substr(0, pos);
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