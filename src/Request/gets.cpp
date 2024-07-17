#include "webserver.hpp"

int Request::getClientFd(){
	return (_clientFd);
}

std::vector<std::pair<std::string, std::string>> Request::getContent(){
	return (_request);
}

std::string Request::getRequestStr(){
	return (_request_str);
}

std::string Request::getFile(){
	return _file;
}

int	Request::getRequestPort()
{
	std::string host = this->getValues("Host");
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

s_domain Request::getRequestDomain(){
	s_domain Domain;
	std::string host = this->getValues("Host");
	
	size_t colonIndex = host.find_last_of(":");
	if (colonIndex != std::string::npos)
	{
		std::string port = host.substr(colonIndex + 1);
		Domain.port = std::stoi(port);
		std::string ip = host.substr(0, colonIndex);
		// std::cout << "HOST\t"  << ip << "\tPORT\t" << port << std::endl;
		if (ip == "localhost")
			ip = "127.0.0.1";
		Domain.host = configIP(ip);
		return (Domain);
	}
	throw (Exception("Port not found", 404));	
}

// std::string	Request::getRequestHost()
// {
// 	std::string host = this->getValues("Host");
// 	std::string port;
// 	size_t colonIndex = host.find_last_of(":");
// 	if (colonIndex != std::string::npos)
// 	{
// 		port = host.substr(colonIndex + 1);
// 		return (port);
// 	}
// 	return ("");
// }

std::string Request::getFileNameProtected( void ){
	if (_file != "/")
		return _file;
	return (_config.getIndex());
}

//WTF ? Why the hell am returning the file name only if it's a post or get
//Return the first word after GET (usually the html) otherwise empty
std::string Request::getFileName( void )
{
	std::string val = getValues("GET");
	if (val.empty()){
		val = getValues("POST");
		if (val.empty())
			return "";
	}
	std::string html_file = firstWord(val);
	if (html_file == "/")
		return (_config.getIndex());
	return html_file;
}


//Return the value of the found key, otherwise empty string
std::string Request::getValues(std::string key){
	for (const auto& pair : _request)
		if (pair.first == key) return pair.second;
	return "";
}

// Method[3], 0 = Methode(Post, delete, etc..), 1 = Link (html/ cgi/ .ico), 2 = HTTP version
std::string Request::getMethod(int index){
	if (index < 0 || static_cast<std::vector<std::string>::size_type>(index) >= _method.size())
		return "";
	return _method[index];
}

std::string Request::getResponse(void){
	std::string response_header;
	response_header = getMethod(2) + " 200 OK\r\nContent-Type: " + getMethod(1) + "\r\n\r\n";
	std::cout << "getmethod for the Response" << response_header;
	return response_header;
}

Server	Request::getConfig(){
	return _config;
}

// Setters

void	Request::setConfig(Server &config)
{
	_config = config;
}