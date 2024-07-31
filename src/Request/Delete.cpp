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
    std::size_t jsonStart = httpRequest.find("\r\n\r\n");
    if (jsonStart == std::string::npos) {
        return ""; // Invalid request, no body found
    }
    jsonStart += 4; // Move past the "\r\n\r\n"

    // Extract the JSON body
    std::string jsonBody = httpRequest.substr(jsonStart);

    // Find the position of the filename in the JSON body
    std::size_t filenamePos = jsonBody.find("\"filename\":\"");
    if (filenamePos == std::string::npos) {
        return ""; // No filename found
    }

    // Move the position to the start of the actual filename
    filenamePos += 12; // Move past "\"filename\":\""

    // Find the closing quote of the filename
    std::size_t endQuotePos = jsonBody.find("\"", filenamePos);
    if (endQuotePos == std::string::npos) {
        return ""; // No closing quote found for the filename
    }

    // Extract the filename
    std::string filename = jsonBody.substr(filenamePos, endQuotePos - filenamePos);

    return filename;
}

void Request::handleDelete(std::string path, std::string file){

	if (file == "")
		throw (HTTPError(OK));

	if (!exists(path + file))
		throw (HTTPError(PAGE_NOT_FOUND));

	// if (!verifyPath(path))
	// 	throw (HTTPError(PAGE_NOT_FOUND));

	Logger::log("File exist and will be deleted", INFO);

	if (std::filesystem::is_regular_file(path + file))
		return (remove(path + file));
	else if (std::filesystem::is_directory(path + file))
		return (handleDirDelete(path + file));
	else
		throw (HTTPError(NO_CONTENT));
	return;
}