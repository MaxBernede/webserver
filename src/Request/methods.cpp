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
		printColor(RED, "Request constructor called ");
		fill_boundary(_request_text);
		_request = parse_response(_request_text);
		for (const auto& pair : _request) {
			std::cout << pair.first << ": " << pair.second << std::endl;
		}
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