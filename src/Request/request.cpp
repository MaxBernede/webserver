#include "webserver.hpp"
#include "request.hpp"

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
std::vector<std::pair<std::string, std::string>> parse_response(const std::string& headers) {
	std::vector<std::pair<std::string, std::string>> request;

	std::istringstream iss(headers);
	std::string line;

	std::getline(iss, line);
	size_t pos = line.find(' ');
	if (pos != std::string::npos)
		request.emplace_back(create_pair(line, pos));
	while (std::getline(iss, line)) {
		size_t pos = line.find(':');
		if (pos != std::string::npos)
			request.emplace_back(create_pair(line, pos));
	}
	return request;
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

void Request::readRequest()
{
	char buffer[BUFFER_SIZE];
	int rb;

	rb = read(_clientFd, buffer, BUFFER_SIZE - 1);
	if (rb < 0){
		std::cerr << "Error reading request" << std::endl;
		return;
	}
	buffer[rb] = '\0';
	_request_text += buffer;;
	if (rb < BUFFER_SIZE - 1)
	{
		doneReading = true;
		printColor(RED, "Request constructor called ");
		fill_boundary(_request_text);
		_request = parse_response(_request_text);
		for (const auto& pair : _request) {
			std::cout << pair.first << ": " << pair.second << std::endl;
		}
	}
}

//Return the value of the found key, otherwise empty string
std::string Request::get_values(std::string key){
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
std::string Request::getFileName( void )
{
	std::string val = get_values("GET");
	if (val.empty()){
		val = get_values("POST");
		if (val.empty())
			return "";
	}
	std::string html_file = firstWord(val);
	if (html_file == "/")
		return "/index.html"; //TODO: make it modular according to config
	return html_file;
}

bool Request::isCgi()
{
	// check extension x.substr(x.find_last_of("*******") + 2) == "cx")
	std::string fileName = getFileName();
	size_t dotIndex = fileName.find_last_of(".");
	if (dotIndex != std::string::npos)
	{
		std::string extension = fileName.substr(dotIndex + 1);
		return (extension == "cgi");
	}
	return (false);
}

int Request::getClientFd()
{
	return (_clientFd);
}

std::vector<std::pair<std::string, std::string>> Request::getContent()
{
	return (_request);
}

std::string Request::getRequestStr()
{
	return (request_str);
}

int	Request::getRequestPort()
{
	std::string host = this->get_values("Host");
	std::string port;
	size_t colonIndex = host.find_last_of(":");
	if (colonIndex != std::string::npos)
	{
		port = host.substr(colonIndex + 1);
		int p = std::stoi(port);
		return (p);
	}
	return (-1);
}

bool Request::isDoneReading()
{
	return (doneReading);
}