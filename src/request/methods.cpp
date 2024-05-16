#include "request.hpp"

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