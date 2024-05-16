#include "webserver.hpp"
#include "request.hpp"

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

//check for localhost::aeewa
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

//Return the first word after GET (usually the html) otherwise empty
std::string Request::getFileName( void )
{
	std::string file;
	std::string val = get_values("GET");
	if (val.empty())
		val = get_values("POST");
	file = val.empty() ? "" : firstWord(val);
	if (file == "/")
		file = "index.html";
	return file;
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