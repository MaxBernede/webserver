#include "Request.hpp"

std::string Request::getEndPath(){
	std::string path = getPath();
	try {
		Location loc = _config.getLocationByName("upload/");
		path += loc.getRoot();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		path += _config.getRoot();
	}
	//Logger::log("Total path is : " + path);
	return path;
}

//check if location, then return true or false
bool Request::methodAccepted(std::string method){
	try {
		Location loc = _config.getLocationByName("upload/");
		if (method == "DELETE")
			return loc.getMethod(DELETE);
		if (method == "POST")
			return loc.getMethod(POST);
	}
	catch (const std::runtime_error& e) {
		if (method == "DELETE")
			return _config.getMethod(DELETE);
		if (method == "POST")
			return _config.getMethod(POST);
	}
	return false;
}

std::string Request::findFileName(std::string method){
	if (method == "DELETE")
		return getDeleteFilename(_requestText);
	if (method == "POST")
		return getFileNameFromBody(getValues("Body"));
	return "";
}

bool checkInsecure(std::string fileName){
	if (fileName.find("..") != std::string::npos)
		return true;
	return false;
}

void Request::execAction(){
	std::string method = getMethod(0);
	Logger::log("Request exec: " + method, INFO);

	if (method != "DELETE" && method != "POST")
		return;

	if (!methodAccepted(method)) //check if method not accepted
		throw HTTPError(METHOD_NOT_ALLOWED);

	std::string path = getEndPath(); // the path before filename 
	Logger::log("endpath is : " + path, WARNING);

	std::string fileName = findFileName(method);
	Logger::log("filename is : " + fileName, WARNING);

	//Check size should have been already be done
	if (checkInsecure(fileName) || checkInsecure(path))
		throw HTTPError(BAD_REQUEST); //Dangerous request

	if (method == "POST" && exists(path + fileName))
		throw HTTPError(CONFLICT);

	createDirIfNoExist(path);

	if (method == "DELETE")
		handleDelete(path, fileName);
	if (method == "POST")
		handlePost(path, fileName);
}
