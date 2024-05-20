#include "webserver.hpp"

int Request::getClientFd(){
	return (_clientFd);
}

std::vector<std::pair<std::string, std::string>> Request::getContent(){
	return (_request);
}

std::string Request::getRequestStr(){
	return (request_str);
}

std::string Request::getFile(){
	return _file;
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

std::string	Request::getRequestHost()
{
	std::string host = this->get_values("Host");
	std::string port;
	size_t colonIndex = host.find_last_of(":");
	if (colonIndex != std::string::npos)
	{
		port = host.substr(colonIndex + 1);
		return (port);
	}
	return ("");
}

//Return the first word after GET (usually the html) otherwise empty
std::string Request::getFileName( void )
{
	//! the GetFileName is almost the same as getFile as setFile do the same,
	//!need to check with yzaim if this one is necesarry about the config ?
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


//Return the value of the found key, otherwise empty string
std::string Request::get_values(std::string key){
	for (const auto& pair : _request)
		if (pair.first == key) return pair.second;
	return "";
}

// Method[3], 0 = Methode(Post, delete, etc..), 1 = Link (html/ cgi/ .ico), 2 = HTTP version
std::string Request::get_method(int index){
	if (index < 0 || static_cast<std::vector<std::string>::size_type>(index) >= _method.size())
		return "";
	return _method[index];
}