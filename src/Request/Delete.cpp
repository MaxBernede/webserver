#include "Request.hpp"


void Request::handleDirDelete(std::string path){
	if (path.back() != '/')
		throw (HTTPError(CONFLICT));

	if (access(path.c_str(), W_OK) != 0)
		throw (HTTPError(FORBIDDEN));

	removeDir(path);
}

void Request::remove(std::string path)
{
	if (access(path.c_str(), W_OK) != 0)
		throw (HTTPError(FORBIDDEN));

	if (std::remove(path.c_str()) == 0)
	{
		Logger::log("File deleted successfully", INFO);
		throw (HTTPError(NO_CONTENT));
	}
	throw (HTTPError(OK));
}

void Request::removeDir(std::string path){
	try {
		std::size_t num = std::filesystem::remove_all(path);
		Logger::log("Removed: " + std::to_string(num) + " total files", INFO);
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error removing directory: " << e.what() << "\n";
		throw (HTTPError(INTERNAL_SRV_ERR));
	}
	throw (HTTPError(NO_CONTENT));
}

std::string Request::getDeleteFilename(const std::string& httpRequest) {
    // Find the start of the JSON body
	//Logger::log(httpRequest);
    std::size_t jsonStart = httpRequest.find("\r\n\r\n");
    if (jsonStart == std::string::npos)
        throw HTTPError(BAD_REQUEST);
    jsonStart += 4; // Move past the "\r\n\r\n"

    std::string jsonBody = httpRequest.substr(jsonStart);

    // Find the position of the filename in the JSON body
    std::size_t filenamePos = jsonBody.find("\"filename\":\"");
    if (filenamePos == std::string::npos)
        throw HTTPError(BAD_REQUEST); 

    filenamePos += 12; // Move past "\"filename\":\""

    std::size_t endQuotePos = jsonBody.find("\"", filenamePos);
    if (endQuotePos == std::string::npos)
        throw HTTPError(BAD_REQUEST); 

    return jsonBody.substr(filenamePos, endQuotePos - filenamePos);
}

void Request::handleDelete(std::string path, std::string file){

	if (file == "")
		throw (HTTPError(OK));

	std::string fullPath = path + file;

	if (!exists(fullPath))
		throw (HTTPError(PAGE_NOT_FOUND));

	Logger::log("File exist and will be deleted", INFO);
	try {
		if (std::filesystem::is_regular_file(fullPath))
			remove(fullPath);
		else if (std::filesystem::is_directory(fullPath))
			handleDirDelete(fullPath);
		else
			throw (HTTPError(NO_CONTENT));
	}
	catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        throw HTTPError(INTERNAL_SRV_ERR);
    }
}