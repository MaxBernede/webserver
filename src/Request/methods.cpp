#include "webserver.hpp"
#include <utils.hpp>

void Request::readRequest()
{
	char buffer[BUFFER_SIZE];
	ssize_t rb;

	// rb = read(_clientFd, buffer, BUFFER_SIZE - 1);
	rb = recv(_clientFd, buffer, (BUFFER_SIZE - 1), MSG_DONTWAIT | MSG_CMSG_CLOEXEC);
	if (rb < 0){
		std::cerr << "Error reading request" << std::endl;
		return;
	}
	buffer[rb] = '\0';
	_read_bytes += rb;
	_request_text += std::string(buffer, rb);
	if (_request_text.length() > _config.getMaxBody())
		throw Exception("Payload too large", 413);
	if (rb < BUFFER_SIZE - 1)
	{
		_doneReading = true;
		constructRequest();
	}
}

bool Request::isRedirect(){
	std::list<s_redirect> redirs = _config.getRedirect();
	for (s_redirect r : redirs){
		std::cout << getFileName() << "\t\t" << r.redirFrom << std::endl;
		if (getFileName() == r.redirFrom){
			std::cout << "is redirect" << std::endl;
			return true;}
	}
	return false;
}

bool Request::isCgi()
{
	// check extension x.substr(x.find_last_of("*******") + 2) == "cx")
	std::string fileName = getFileName();
	std::string extension = getExtension(fileName);
	return (extension == "cgi");
}


bool Request::isDoneReading()
{
	return (_doneReading);
}

//check that it's not the Content-Type defined line
//then return false or true if boundary found
bool Request::isBoundary(const std::string &line){
	if (line.find("Content-Type") != std::string::npos)
		return false;
	return line.find(_boundary) != std::string::npos;
}


void Request::printAllData(){
	printColor(YELLOW, "All the data on the Request Class :");
	std::cout << "\tBoundary: " << _boundary << std::endl;
	std::cout << "\tMethod: ";
	for (const auto &method : _method)
		std::cout << method << "\n";
	std::cout << std::endl << std::endl;
	for (const auto& pair : _request)
		printColor(RESET, pair.first, ": ", pair.second);
	std::cout << std::endl << std::endl;
}

bool Request::redirRequest405() // If Method not Allowed, redirects to Server 405
{
	std::string method = getMethod(0);
	int index = -1;
	
	if (method == "GET")
		index = GET;
	else if (method == "POST")
		index = POST;
	else if (method == "DELETE")
		index = DELETE;
	if (!_config.getMethod(index))
	{
		int found = false;
		for (auto item : _config.getErrorPages())
		{
			if (item.err == 405)
			{
				found = true;
				_file = item.url; // redir to error page on Server
			}
		}
		std::string filepath = _config.getRoot() + _file;
		if (access(filepath.c_str(), F_OK) == -1 || !found) // redirect to hardcoded 405 error
		{
			return (false);
		}
	}
	return (true);
}

bool Request::redirRequest404()
{
	std::string root = _config.getRoot();
	std::string filepath = root + _file;
	std::cout << "checking file: " << filepath << std::endl;
	std::cout << "FILE: " << _file << std::endl;
	if (access(filepath.c_str(), F_OK) == -1) // If file does not exist
	{
		int found = false;
		for (auto item : _config.getErrorPages())
		{
			if (item.err == 404)
			{
				found = true;
				_file = item.url; // redir to error page on Server
			}
		}
		std::cout << "_file after looping errorpgs: " << _file << std::endl;
		filepath = root + _file;
		if (access(filepath.c_str(), F_OK) == -1 || !found)
		{
			return (false); // redirect to hardcoded 404 error
		}
	}
	return (true);
}

int Request::checkRequest() // Checking for 404 and 405 Errors
{
	std::cout << "CHECKING REQUEST!!\n";
	if (!redirRequest405())
	{
		std::cout << "405!\n";
		return (405);
	}
	if (!redirRequest404())
	{
		std::cout << "404!\n";
		return (404);
	}
	std::cout << "0!\n";
	return (0);
}

// void Request::checkRequest()
// {
// 	std::string method = getMethod(0);
// 	int index = -1;
// 	std::cout << "CHECKING    Method: " << method << std::endl;
	
// 	if (method == "GET")
// 		index = GET;
// 	else if (method == "POST")
// 		index = POST;
// 	else if (method == "DELETE")
// 		index = DELETE;
// 	if (!_config.getMethod(index))
// 	{
// 		// int found = false;
// 		std::cout << "Method: " << method << " ";
// 		for (auto item : _config.getErrorPages())
// 		{
// 			if (item.err == 405)
// 			{
// 				// found = true;
// 				_file = item.url;
// 			}
// 		}
			
// 		//throw(Exception("Method not allowed", 1));
// 	}
// }

void	Request::configConfig(){
	std::string temp = _method[1];
	// std::cout << "TEMP\t" << temp << std::endl;
	if (temp.find('/', 1) != std::string::npos)
		temp.erase(temp.find('/', 1));
	// std::cout << "TEMP\t" << temp << std::endl;
	std::list<Location> locs = _config.getLocation();
	for (Location loc : locs){
		// std::cout << "locname\t" << loc.getName() << std::endl;
		if (temp == loc.getName()){
			_config.setRoot(loc.getRoot());
			for (int i = GET; i <= TRACE; i++)
				_config.setMethod(loc.getMethod(i), i);
			_config.setRedirect(loc.getRedirect());
			_config.setAutoIndex(loc.getAutoIndex());
			_config.setIndex(loc.getIndex());
			_config.setCGI(loc.getCGI());
			_config.setPath(loc.getPath());
			break ;
		}
	}
	// std::cout << "new?:\t" << _config.getRoot() << std::endl;
}