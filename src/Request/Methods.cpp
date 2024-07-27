#include "Request.hpp"

// Throw HTTPError if the request at this moment is bigger than MAX BODY SIZE
void Request::requestReadTooLong(){
	if (_request_text.length() > MAX_BODY_SIZE)
	{
		_doneReading = true;
		throw HTTPError(URI_TOO_LONG);
	}
}

void Request::startConstruRequest(){
	try {
		constructRequest();
	}
	catch (const RequestException& e) {
		e.what();
		throw HTTPError(_errorCode);
	}
}

void Request::readRequest()
{
	char buffer[BUFFER_SIZE];

	int rb = read(_clientFd, buffer, BUFFER_SIZE - 1);

	if (rb < 0){
		_errorCode = ErrorCode::BAD_REQUEST; //not sure of the error
		_doneReading = true;
		std::cerr << "Error reading request fd: " << _clientFd << " : " << std::strerror(errno) << std::endl;

		throw (HTTPError(ErrorCode::BAD_REQUEST));
	}
	buffer[rb] = '\0';
	_recv_bytes += rb;
	_request_text += std::string(buffer, rb);
	
	requestReadTooLong();
	
	if (rb < BUFFER_SIZE - 1) // Finished reading
		_doneReading = true;
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
	//Logger::log("Is done reading " + std::to_string(_doneReading), INFO);
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
	std::cout << "Boudary: " << _boundary << std::endl;
	std::cout << "Method: ";
	for (const auto &method : _method)
		std::cout << method << " ";
	std::cout << std::endl;
	for (const auto& pair : _request)
		Logger::log(pair.first + ": " + pair.second, DEBUG);
}

// Void or Throw HTTPError
void Request::redirRequest501()
{
	std::string method = getMethod(0);
	if (method != "GET" && method != "POST" && method != "DELETE" && method != "HEAD"){
		_errorCode = METHOD_NOT_IMPLEMENTED;
		throw(HTTPError(ErrorCode::METHOD_NOT_IMPLEMENTED));
	}
}

// Void or Throw HTTPError
void Request::redirRequest405() // If Method not Allowed, redirects to Server 405
{
	std::string method = getMethod(0);
	int index = -1;
	
	if (method == "GET")
		index = GET;
	else if (method == "POST")
		index = POST;
	else if (method == "DELETE")
		index = DELETE;
	else if (method == "HEAD")
		index = HEAD;
	if (index == HEAD)
		return ;
	if (index != -1 && _config.getMethod(index) == false)
		throw(HTTPError(ErrorCode::METHOD_NOT_ALLOWED));

}

void Request::searchErrorPage()
{
	int found = false;
	for (auto item : _config.getErrorPages())
	{
		if (item.err == _errorCode)
		{
			found = true;
			_errorPageFound = true;
			_file = item.url; // redir to error page on Server
		}
	}
	_filePath = _config.getRoot() + _file;
	if (access(_filePath.c_str(), F_OK) == -1 || !found)
	{
		_errorPageFound = false;
	}
}

// Void or throw HTTPError 404
void Request::redirRequest404()
{
	if (_file == "")
		_file = _config.getIndex();
	_filePath = _config.getRoot() + _file;
	bool dirListing = _config.getAutoIndex();
	if (access(_filePath.c_str(), F_OK) == -1 && dirListing == false)
		throw (HTTPError(ErrorCode::PAGE_NOT_FOUND));
}

// Not sure about this logic
void	Request::handleDirListing()
{
	if (_file == "")
		_file = _config.getIndex();
	if ((_file == "" || _file.back() == '/') && !_config.getAutoIndex())
		throw (HTTPError(ErrorCode::PAGE_NOT_FOUND));
	else if ((_file == "" || _file.back() == '/') && _config.getAutoIndex())
}

void Request::checkRequest() // Checking for 404 and 405 Errors
{
	Logger::log("Checking file...", LogLevel::INFO);

	redirRequest405(); // ---> throw something case error
	redirRequest501();
	handleRedirection();
	handleDirListing(); // NOT WORKING
	redirRequest404();
}

void Request::remove(std::string &path)
{
	if (access(path.c_str(), W_OK) != 0)
	{
		_errorCode = FORBIDDEN;
		throw RequestException("403: Path to delete have no write access", LogLevel::ERROR);
	}
	if (std::remove(path.c_str()) == 0)
	{
		Logger::log("File deleted successfully", INFO);
		_errorCode = NO_CONTENT;
		throw RequestException("204: Should return Success");
	}
	throw RequestException("Failed to delete the file", LogLevel::ERROR);
}

void Request::removeDir(std::string &path){
	try {
		std::size_t num = std::filesystem::remove_all(path);
		Logger::log("Removed: " + std::to_string(num) + " total files", INFO);
		_errorCode = NO_CONTENT;
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error removing directory: " << e.what() << "\n";
		_errorCode = INTERNAL_SRV_ERR;
		throw RequestException("500: Error while deleting the dir", LogLevel::WARNING);
	}
	throw RequestException("204: Should return Success");
}

void Request::handleDirDelete(std::string &path){
	if (path.back() != '/'){
		_errorCode = CONFLICT;
		throw RequestException("409: Path to delete doesn't end with '/'", ERROR);
	}
	if (access(path.c_str(), W_OK) != 0){
		_errorCode = FORBIDDEN;
		throw RequestException("403: Path to delete has no write access", ERROR);
	}
	removeDir(path);
}

void Request::handlePost()
{
	std::string body = getValues("Body");

	if (body.empty())
		throw RequestException("Body is empty", ERROR); // check if not 422

	Logger::log("Creating the file", INFO);
	createFile(body, getPath() + "/saved_files");
}

void Request::handleDelete(){
	std::string file = getDeleteFilename(_request_text);
	std::string path = getPath() + "/saved_files/" + file;

	if (file == "")
		throw RequestException("File name is empty, nothing will be deleted", LogLevel::WARNING);

	Logger::log("File to delete is : " + file + " Path: " + path, INFO);
	if (!exists(path)){
		_errorCode = ErrorCode::PAGE_NOT_FOUND;
		throw RequestException("File not deleted: doesn't exist", LogLevel::WARNING);
	}

	if (!verifyPath(path)){
		//Not sure of error code
		_errorCode = ErrorCode::PAGE_NOT_FOUND;
		throw RequestException("Path requiered not found, file tried to delete ../", LogLevel::ERROR);
	}
	Logger::log("File exist and will be deleted", INFO);

	if (std::filesystem::is_regular_file(path))
		return (remove(path));
	else if (std::filesystem::is_directory(path))
		return (handleDirDelete(path));
	else
		throw RequestException("Path is not a file and not a dir", LogLevel::ERROR);
	return;
}

// TODO not sure if we need this function anymore
void Request::handleRedirection(){
	std::list<s_redirect> redirs = _config.getRedirect();
	std::string fileName = getFileNameProtected();

	for (s_redirect r : redirs){
		// std::cout << fileName << "\t\t" << r.redirFrom << std::endl;
		if (fileName == r.redirFrom){
			Logger::log("Is a redirect", LogLevel::WARNING);
			throw(HTTPError(ErrorCode(r.returnValue)));
		}
	}
}

void	Request::configConfig(){
	std::string temp = getFileNameProtected();
	if (temp.find('/', 1) != std::string::npos)
		temp.erase(temp.find('/', 1) + 1);
	std::list<Location> locs = _config.getLocation();
	for (Location loc : locs){
		std::cout << "TEMP\t" << temp << "\tLOC\t" << loc.getName() << std::endl;
		if (temp == loc.getName() || (temp == loc.getRoot() && loc.getRoot() != _config.getRoot())){
			std::cout << "IS LOCATION" << std::endl;
			_config.setRoot(loc.getRoot());
			for (int i = GET; i <= TRACE; i++)
				_config.setMethod(loc.getMethod(i), i);
			_config.setRedirect(loc.getRedirect());
			_config.setAutoIndex(loc.getAutoIndex());
			_config.setIndex(loc.getIndex());
			_config.setCGI(loc.getCGI());
			_config.setPath(loc.getPath());
			std::string newName = getFileNameProtected();
			newName.erase(0, temp.length());
			std::cout << newName << std::endl;
			_file = newName;
			break ;
		}
	}
}

void Request::checkVersion(){
	std::string v = getMethod(2);

	// std::cout << getMethod(0) << getMethod(1) << getMethod(2) <<std::endl;
	// std::cout << v << " " << v[5] << "<-- version" << std::endl;
	if (v[5] != '1' && v[5] != '2'){
		_method[2] = "HTTP/1.1";
		_errorCode = ErrorCode::HTTP_NOT_SUPPORT;
		throw (RequestException("HTTP version is not correct", LogLevel::ERROR));
	}
}

void Request::checkErrors(){
	checkVersion();
}

void Request::execAction(){
	std::string method = getMethod(0);

	Logger::log("Request exec: " + method, INFO);

	if (method == "DELETE")
		handleDelete();
	if (method == "POST")
		handlePost();
}
