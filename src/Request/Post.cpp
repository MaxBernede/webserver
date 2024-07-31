#include "Request.hpp"


void Request::handlePost(std::string path, std::string file)
{
	std::string body = getValues("Body");

	if (body.empty())
		throw (HTTPError(OK)); // check if not 422

	Logger::log("Creating the file", INFO);
	createFile(body, path, file);
}
