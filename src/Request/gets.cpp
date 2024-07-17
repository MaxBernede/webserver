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
	//std::cout << "\n\nHost: " << host << std::endl;
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
	std::string host = this->getValues("Host");
	std::string port;
	size_t colonIndex = host.find_last_of(":");
	if (colonIndex != std::string::npos)
	{
		port = host.substr(colonIndex + 1);
		return (port);
	}
	return ("");
}

std::string Request::getFileNameProtected( void ){
	Logger::log("GetFileNameProtected is: " + _file, INFO);
	if (_file != "/")
		return _file;
	for (auto item : _config.getIndex())
	{
		if (getExtension(item) == "html")
			return (item);
	}
	//!TAKE CARE IF NOT FOUND IN CONFIG FILE PROBLEM
	return "";
}


//WTF ? Why the hell am ireturning the file name only if it's a post or get
//Return the first word after GET (usually the html) otherwise empty
std::string Request::getFileName( void )
{
	return _file;
	// std::string val = getValues("GET");
	// if (val.empty()){
	// 	val = getValues("POST");
	// 	if (val.empty())
	// 		return "";
	// }
	// std::string html_file = firstWord(val);
	// if (html_file == "/")
	// 	return *(_config.getIndex()).begin(); //TODO: make it modular according to config
	// return html_file;
}


//Return the value of the found key, otherwise empty string
std::string Request::getValues(std::string key){
	for (const auto& pair : _request)
		if (pair.first == key) return pair.second;
	return "";
}

// TODO make an enum for this, remove '/r' char from the end of the lines
// Method[3], 0 = Methode(Post, delete, etc..), 1 = Link (html/ cgi/ .ico), 2 = HTTP version
std::string Request::getMethod(int index){
	if (index < 0 || static_cast<std::vector<std::string>::size_type>(index) >= _method.size())
		return "";
	return _method[index];
}

Server	Request::getConfig(){
	return _config;
}

// Setters

void	Request::setConfig(Server config)
{
	_config = config;
}

std::string Request::getDeleteFilename(const std::string& httpRequest) {
    // Find the start of the JSON body
    std::size_t jsonStart = httpRequest.find("\r\n\r\n");
    if (jsonStart == std::string::npos) {
        return ""; // Invalid request, no body found
    }
    jsonStart += 4; // Move past the "\r\n\r\n"

    // Extract the JSON body
    std::string jsonBody = httpRequest.substr(jsonStart);

    // Find the position of the filename in the JSON body
    std::size_t filenamePos = jsonBody.find("\"filename\":\"");
    if (filenamePos == std::string::npos) {
        return ""; // No filename found
    }

    // Move the position to the start of the actual filename
    filenamePos += 12; // Move past "\"filename\":\""

    // Find the closing quote of the filename
    std::size_t endQuotePos = jsonBody.find("\"", filenamePos);
    if (endQuotePos == std::string::npos) {
        return ""; // No closing quote found for the filename
    }

    // Extract the filename
    std::string filename = jsonBody.substr(filenamePos, endQuotePos - filenamePos);

    return filename;
}

ErrorCode Request::getErrorCode(){
	return _errorCode;
}

std::string Request::getErrorString(){
	auto it = httpStatus.find(_errorCode);
	if (it != httpStatus.end()) {
		return it->second;
	}
	return "Unknown Error";
}