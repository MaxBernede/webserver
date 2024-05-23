#include "webserver.hpp"
#include <utils.hpp>

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
	printColor(YELLOW, "All the datas on the Request Class :");
	std::cout << "Boudary: " << _boundary << std::endl;
	std::cout << "Method: ";
	for (const auto &method : _method)
		std::cout << method << " ";
	std::cout << std::endl;
	for (const auto& pair : _request)
		printColor(RESET, pair.first, ": ", pair.second);
}

void Request::checkRequest()
{
	std::string method = getMethod(0);
	int index = -1;
	std::cout << "CHECKING    Method: " << method << std::endl;
	
	if (method == "GET")
		index = GET;
	else if (method == "POST")
		index = POST;
	else if (method == "DELETE")
		index = DELETE;
	if (!_config.getMethod(index))
	{
		int found = false;
		std::cout << "Method: " << method << " ";
		for (auto item : _config.getErrorPages())
		{
			if (item.err == 405)
			{
				found = true;
				_file = item.url;
			}
		}
			
		//throw(Exception("Method not allowed", 1));
	}
}