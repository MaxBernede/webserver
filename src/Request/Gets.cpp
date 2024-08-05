#include "Request.hpp"

int Request::getClientFd(){
	return (_clientFd);
}

std::vector<std::pair<std::string, std::string>> Request::getContent(){
	return (_request);
}

std::string Request::getRequestStr(){
	return (_request_text);
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
	if (_file != "")
		return _file;
	return (_config.getIndex());
}

//Return the first word after GET (usually the html) otherwise empty
std::string Request::getFileName( void ){
	return _file;
}

//Return the value of the found key, otherwise empty string
std::string Request::getValues(std::string key){
	for (const auto& pair : _request)
		if (pair.first == key) return pair.second;
	return "";
}

// Method[3], 0 = Methode(Post, delete, etc..), 1 = Link (html/ cgi/ .ico), 2 = HTTP version
std::string Request::getMethod(size_t index){
	if (index < 0 || index >= _method.size())
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

void Request::setErrorCode(ErrorCode err)
{
	_errorCode = err;
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

s_domain Request::getRequestDomain(){
	s_domain Domain;
	std::string host = this->getValues("Host");
	size_t colonIndex = host.find_last_of(":");
	if (colonIndex != std::string::npos)
	{
		std::cout << "HOST: " << host << std::endl;
		std::string port = host.substr(colonIndex + 1);
		std::cout << "port: " << port << std::endl;
		Domain.port = std::stoi(port);
		std::string ip = host.substr(0, colonIndex);
		std::cout << "ip: " << ip << std::endl;
		// std::cout << "HOST\t"  << ip << "\tPORT\t" << port << std::endl;
		if (ip == "localhost")
			ip = "127.0.0.1";
		Domain.host = ip;
		return (Domain);
	}
	throw (HTTPError(INTERNAL_SRV_ERR));
}

std::string	Request::getFilePath()
{
	return (_filePath);
}

bool Request::getErrorPageStatus()
{
	return (_errorPageFound);
}

std::string	Request::getServerName()
{
	return (_config.getName());
}

//True or False, HEAD included so no read after from serverRun
bool Request::isEmptyResponse(){
	std::string m = getMethod(0);

	return (m == "DELETE" || m == "POST" || m == "HEAD");
}

std::string	Request::getBoundary()
{
	return (_boundary);
}
