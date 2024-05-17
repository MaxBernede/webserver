#include "webserver.hpp"

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
		construct_request();
	}
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


bool Request::isDoneReading()
{
	return (doneReading);
}

//check that it's not the Content-Type defined line
//then return false or true if boundary found
bool Request::is_boundary(const std::string &line){
	if (line.find("Content-Type") != std::string::npos)
		return false;
	return line.find(_boundary) != std::string::npos;
}


void Request::print_all_datas(){
	printColor(YELLOW, "All the datas on the Request Class :");
	std::cout << "Boudary: " << _boundary << std::endl;
	std::cout << "Method: ";
	for (const auto &method : _method)
		std::cout << method << " ";
	std::cout << std::endl;
	for (const auto& pair : _request)
		printColor(RESET, pair.first, ": ", pair.second);
}