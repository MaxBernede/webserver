#include "Request.hpp"


//Explain that path is root + path in config
std::string Request::getEndPath() {
	std::string path = getPath();
	path += _config.getPath();
	return path;
}

//check if location, then return true or false
bool Request::methodAccepted(std::string method) {
	if (method == "DELETE")
		return _config.getMethod(DELETE);
	if (method == "POST")
		return _config.getMethod(POST);
	return false;
}

std::string Request::findFileName(std::string method) {
	if (method == "DELETE")
		return getDeleteFilename(_requestText);
	if (method == "POST" && !_boundary.empty()) // POST with a boundary
		return getOtherFilename();
	return "";
}

bool checkInsecure(std::string fileName) {
	return (fileName.find("..") != std::string::npos);
}

void Request::execAction() {
	std::string method = getMethod(0);
	Logger::log("Request exec: " + method, INFO);

	if (method != "DELETE" && method != "POST")
		return;

	//!! this is already checked... remove
	// if (!methodAccepted(method)) //check if method not accepted
	// 	throw HTTPError(METHOD_NOT_ALLOWED);

	std::string path = getEndPath(); // the path before filename 
	Logger::log("endpath is : " + path, WARNING);

	std::string fileName = findFileName(method);
	Logger::log("filename is : " + fileName, WARNING);
	if (fileName.empty() && method == "POST")
	{
		Logger::log("The following data was posted: \n" + getRawBody(), LogLevel::WARNING);
		return ;
	}
	//Check size should have been already be done
	if (checkInsecure(fileName) || checkInsecure(path))
	{
		std::cout << "13\n";
		throw HTTPError(BAD_REQUEST); //Dangerous request
	}

	if (method == "POST" && exists(path + fileName))
		throw HTTPError(CONFLICT);

	createDirIfNoExist(path);

	if (method == "DELETE")
		handleDelete(path, fileName);
	if (method == "POST")
		handlePost(path, fileName);
}
