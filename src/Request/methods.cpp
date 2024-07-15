#include "webserver.hpp"
#include <utils.hpp>
#include <filesystem>

void Request::readRequest()
{
	char buffer[BUFFER_SIZE];
	int rb;

	rb = recv(_clientFd, buffer, BUFFER_SIZE - 1, 0);
	if (rb < 0){
		std::cerr << "Error reading request" << std::endl;
		return;
	}
	buffer[rb] = '\0';
	_recv_bytes += rb;
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
	Logger::log("Application started", INFO);
	//printColor(YELLOW, "All the datas on the Request Class :");
	std::cout << "Boudary: " << _boundary << std::endl;
	std::cout << "Method: ";
	for (const auto &method : _method)
		std::cout << method << " ";
	std::cout << std::endl;
	for (const auto& pair : _request)
		printColor(RESET, pair.first, ": ", pair.second);
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
	if (_file == "/")
		_file = "index.html";
	root = root + "html/";
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

void Request::remove(std::string &path){
	if (std::remove(path.c_str()) == 0)
		Logger::log("File deleted successfully", INFO);
	else
		Logger::log("Failed to delete the file", ERROR);
}

void Request::removeDir(std::string &path){
    try {
        std::size_t num = std::__fs::filesystem::remove_all(path);
        Logger::log("Removed: " + std::to_string(num) + " total files", INFO);
		_errorCode = NO_CONTENT;
		Logger::log("204: Should return Success", INFO);
    } catch (const std::__fs::filesystem::filesystem_error& e) {
        std::cerr << "Error removing directory: " << e.what() << "\n";
		_errorCode = INTERNAL_SRV_ERR;
		Logger::log("500: Error while deleting the dir", WARNING);
    }
}


void Request::handleDirDelete(std::string &path){
	if (path.back() != '/'){
		_errorCode = CONFLICT;
		Logger::log("409: Path to delete doesn't end with '/'", ERROR);
		return;
	}
	if (access(path.c_str(), W_OK) != 0){
		_errorCode = FORBIDDEN;
		Logger::log("403: Path to delete have no write access", ERROR);
		return;
	}
	removeDir(path);
}

void Request::handlePost(){
	std::string body = getValues("Body");

	if (body.empty())
		return (Logger::log("Body is empty", ERROR));

	Logger::log("Creating the file", INFO);
	createFile(body, getPath() + "/saved_files");
}

void Request::handleDelete(){
	std::string file = getDeleteFilename(_request_text);
	std::string path = getPath() + "/saved_files/" + file;

	if (file == ""){
		Logger::log("File is empty, nothing will be deleted", WARNING);
		return;
	}

	Logger::log("File to delete is : " + file + " Path: " + path, INFO);
	if (!exists(path)){
		Logger::log("File doesn't exist", WARNING);
		return;
	}

	Logger::log("File exist and will be deleted", INFO);

	if (std::__fs::filesystem::is_regular_file(path))
    	return (remove(path));
    else if (std::__fs::filesystem::is_directory(path))
        return (handleDirDelete(path));
	Logger::log("Path is not a file and not a dir", ERROR);
	return;
}