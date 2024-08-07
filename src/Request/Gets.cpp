#include "Request.hpp"

int Request::getClientFd() {
	return (_clientFd);
}

std::vector<std::pair<std::string, std::string>> Request::getContent() {
	return (_request);
}

std::string Request::getRequestStr() {
	return (_requestText);
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

std::string Request::getFileNameProtected(void) {
	if (_file != "")
		return _file;
	return (_config.getIndex());
}

//Return the first word after GET (usually the html) otherwise empty
std::string Request::getFileName(void) {
	return _file;
}

//Return the value of the found key, otherwise empty string
std::string Request::getValues(std::string key) {
	for (const auto& pair : _request)
		if (pair.first == key) return pair.second;
	return "";
}

// Method[3], 0 = Methode(Post, delete, etc..), 1 = Link (html/ cgi/ .ico), 2 = HTTP version
std::string Request::getMethod(size_t index) {
	if (index >= _method.size())
		return "";
	return _method[index];
}

Server	Request::getConfig() {
	return _config;
}

ErrorCode Request::getErrorCode() {
	return _errorCode;
}

std::string Request::getErrorString() {
	auto it = httpStatus.find(_errorCode);
	if (it != httpStatus.end())
		return it->second;
	return "Unknown Error";
}

std::string Request::getRawBody()
{
	std::string delimiter = "\r\n\r\n";
	size_t pos = _requestText.find(delimiter);

	if (pos == std::string::npos)
	{
		// Delimiter not found, which implies no body or incorrectly formatted input
		return "";
	}
	// Skip past the delimiter
	pos += delimiter.length();
	// Return the body starting right after the delimiter
	return _requestText.substr(pos);
}


s_domain Request::getRequestDomain(std::string host_val) {
	s_domain Domain;
	size_t colonIndex = host_val.find_last_of(":");
	if (colonIndex != std::string::npos)
	{
		// std::cout << "HOST: " << host << std::endl;
		std::string port = host_val.substr(colonIndex + 1);
		// std::cout << "port: " << port << std::endl;
		Domain.port = std::stoi(port);
		std::string ip = host_val.substr(0, colonIndex);
		// std::cout << "ip: " << ip << std::endl;
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
bool Request::isEmptyResponse() {
	std::string m = getMethod(0);
	return (m == "DELETE" || m == "POST" || m == "HEAD");
}

std::string	Request::getBoundary()
{
	return (_boundary);
}
