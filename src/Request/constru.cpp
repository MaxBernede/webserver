#include "webserver.hpp"
#include "request.hpp"

void Request::setFile(){
	std::string val = get_values("GET");
	if (val.empty())
		val = get_values("POST");
	_file = val.empty() ? "" : firstWord(val);
	if (_file == "/")
		_file = "index.html";
}

//Create a pair out of the line and the int pos of the delimiter (: for every lines or space for the first line)
std::pair<std::string, std::string> create_pair(const std::string &line, size_t pos){
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1); // Skip the delimiter
	if (!value.empty() && value[0] == ' ')
		value = value.substr(1); // Remove leading space if present
	return std::make_pair(key, value);
}

//fill the _request
// TODO clean this class, rename function
void Request::parse_response(const std::string& headers) {

	std::istringstream iss(headers);
	std::string line;

	std::getline(iss, line);
	size_t pos = line.find(' ');
	if (pos != std::string::npos)
		_request.emplace_back(create_pair(line, pos));
	while (std::getline(iss, line)) {
		size_t pos = line.find(':');
		if (pos != std::string::npos)
			_request.emplace_back(create_pair(line, pos));
	}
}

void Request::fill_boundary(std::string text){
	std::string search = "boundary=";
	size_t pos = text.find(search);
	if (pos == std::string::npos){
		_boundary = "";
		return;
	}
	pos += search.size();
	size_t endPos = text.find('\n', pos);
	if (endPos == std::string::npos){
		_boundary = "";
		return;
	}
    _boundary = text.substr(pos, endPos - pos);
}

//Constructor that parses everything
Request::Request(int clientFd) : _clientFd(clientFd), doneReading(false) {}

Request::~Request() {}

void Request::construct_request(){
	printColor(BLUE, "Constructor request call");
	fill_boundary(_request_text);
	parse_response(_request_text);	
	setFile();
}