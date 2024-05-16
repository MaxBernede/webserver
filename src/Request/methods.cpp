#include "webserver.hpp"
#include "request.hpp"

void request::fill_file(){
	std::string val = get_values("GET");
	if (val.empty())
		val = get_values("POST");
	_file = val.empty() ? "" : firstWord(val);
	if (_file == "/")
		_file = "index.html";
}

void request::show_datas(){
	printColor(YELLOW, "All the datas on the Request Class :");
	printColor(RESET, "Boundary: ", _boundary);
	std::cout << "Method: ";
	for (const auto &method : _method)
		std::cout << method << " ";
	std::cout << std::endl;
	for (const auto& pair : _request)
		printColor(RESET, pair.first, ": ", pair.second);
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

//Create a pair out of the line and the int pos of the delimiter (: for every lines or space for the first line)
std::pair<std::string, std::string> create_pair(const std::string &line, size_t pos){
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1); // Skip the delimiter
	if (!value.empty() && value[0] == ' ')
		value = value.substr(1); // Remove leading space if present
	return std::make_pair(key, value);
}


// Parse what is after the \r of the
void request::parse_body(std::istringstream &iss, std::string &line) {
    std::string body = "Body ";

    if (_boundary != "" && is_boundary(line)) {
        while (std::getline(iss, line)) {
            if (_boundary != "" && is_boundary(line)) {
                _request.emplace_back(create_pair(body, 4));
                break;
            }
            body += line;
            body += "\n";
        }
    }
}

//fill the _request
// it works as : get the first line based on space
// then check for the ':' however if there is a boundary and its found, keep everything between as body
void request::parse_response(const std::string& headers) {
	std::istringstream iss(headers);
	std::string line;

	parse_first_line(iss);
	while (std::getline(iss, line)) {
		if (line == "\r")
			break;
		size_t pos = line.find(':');
		if (pos != std::string::npos)
			_request.emplace_back(create_pair(line, pos));
	}
    if (std::getline(iss, line))
        parse_body(iss, line);
}