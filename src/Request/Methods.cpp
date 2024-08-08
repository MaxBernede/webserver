#include "Request.hpp"

// Throw HTTPError if the request at this moment is bigger than MAX BODY SIZE
void Request::requestReadTooLong(){
	if (_requestText.length() > MAX_BODY_SIZE)
	{
		_doneReading = true;
		throw HTTPError(PAYLOAD_TOO_LARGE);
	}
}

void Request::startConstruRequest(){
		constructRequest();
}

void Request::readRequest()
{
	char	buffer[BUFFER_SIZE];
	int		rb = read(_clientFd, buffer, BUFFER_SIZE - 1);

	if (rb < 0){
		_doneReading = true;
		Logger::log("Error reading request", LogLevel::ERROR);
		throw (HTTPError(ErrorCode::BAD_REQUEST));
	}
	buffer[rb] = '\0';
	_recvBytes += rb;
	_requestText += std::string(buffer, rb);
	requestReadTooLong();
	if (rb < BUFFER_SIZE - 1) // Finished reading
		_doneReading = true;
}

bool Request::isCgi()
{
	// check extension x.substr(x.find_last_of("*******") + 2) == "cx")
	std::string fileName = getFileName();
	std::string extension = getExtension(fileName);
	// std::cout << "What is the file extension: " << extension << std::endl;
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
	std::cout << "Boundary: " << _boundary << std::endl;
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
	std::string	method	= getMethod(0);
	int			index	= -1;
	
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
	_filePath = _file;
	if (access(_filePath.c_str(), F_OK) == -1 || !found)
		_errorPageFound = false;
}

// Void or throw HTTPError 404
void Request::redirRequest404()
{
	if (getMethod(0) == "DELETE")
		return ;
	if (_file == "")
		_file = _config.getIndex();
	_filePath = _config.getRoot() + _file;
	Logger::log("file: " + _filePath, LogLevel::INFO);
	bool dirListing = _config.getAutoIndex();
	if (access(_filePath.c_str(), F_OK) == -1 && dirListing == false)
		throw (HTTPError(ErrorCode::PAGE_NOT_FOUND));
	if (!exists(_filePath))
		throw (HTTPError(ErrorCode::PAGE_NOT_FOUND));
}

void	Request::handleDirListing()
{
	if (_file == "")
		_file = _config.getIndex();
	if ((_file == "" || _file.back() == '/') && !_config.getAutoIndex())
		throw (HTTPError(ErrorCode::PAGE_NOT_FOUND));
	else if ((_file == "" || _file.back() == '/') && _config.getAutoIndex())
		throw (HTTPError(ErrorCode::DIRECTORY_LISTING));
}

void Request::checkRequest()
{
	Logger::log("Checking file...", LogLevel::INFO);
	std::cout << "REQUEST:\n" << _requestText << std::endl;
	if (_requestText.size() >= _config.getMaxBody())
		throw (HTTPError(PAYLOAD_TOO_LARGE));
	redirRequest405(); // ---> throw something case error
	redirRequest501();
	handleRedirection();
	redirRequest404();
	handleDirListing();
}


void Request::handleRedirection(){
	std::list<s_redirect> redirs = _config.getRedirect();
	std::string fileName = getFileNameProtected();

	for (s_redirect r : redirs){
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
			std::cout << "LOCATION CHECK\t" << temp << "\t" << loc.getRoot() << "\t" << _config.getRoot() << std::endl;
		if (temp == loc.getName() || (temp == loc.getRoot() && loc.getRoot() != _config.getRoot())){
			// std::cout << "LOCATION FOUND" << temp << "\t" << loc.getRoot() << "\t" << _config.getRoot() << std::endl;
			_config.setRoot(_config.getRoot() + loc.getRoot());
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

	if (v[5] != '1' && v[5] != '2'){
		_method[2] = "HTTP/1.1";
		throw (HTTPError(HTTP_NOT_SUPPORT));
	}
}

void Request::checkErrors(){
	checkVersion();
}
