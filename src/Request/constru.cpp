#include "webserver.hpp"
#include "request.hpp"


void Request::setFileName(std::string newName){
	_file = newName;
}

// Max : is it maintanable ?
void Request::setFile() {
	_file = getMethod(1);
	_file.erase(0, 1);
	if (_file == "")
	{
		_file = _config.getIndex();
	}
	// _file = "index.html";
}

//Create a pair out of the line and the int pos of the delimiter (: for every lines or space for the first line)
std::pair<std::string, std::string> create_pair(const std::string &line, size_t pos){
	std::string key		= line.substr(0, pos);
	std::string value	= line.substr(pos + 1); // Skip the delimiter

	if (!value.empty() && value[0] == ' ')
		value = value.substr(1); // Remove leading space if present
	return std::make_pair(key, value);
}

void Request::parseBody(std::istringstream &iss, std::string &line) {
    std::string body	= "Body ";

    if (_boundary != "" && isBoundary(line)) {
        while (std::getline(iss, line)) {
            if (_boundary != "" && isBoundary(line)) {
                _request.emplace_back(create_pair(body, 4));
                break;
            }
            body += line;
            body += "\n";
        }
    }
}


//save as well the GET request in the all datas AND the _method
void Request::parseFirstLine(std::istringstream &iss){
	std::string	line, arg;

	std::getline(iss, line);
    std::istringstream line_stream(line);

    while (std::getline(line_stream, arg, ' ')){
        _method.push_back(arg);
	}
	// setFileName(_method[1]);
	if (_method.size() < 3)
		throw (Exception("method not found", 404));
	_method[2].erase(std::remove(_method[2].begin(), _method[2].end(), '\r'), _method[2].end());
	size_t pos = line.find(' ');
	if (pos != std::string::npos)
		_request.emplace_back(create_pair(line, pos));
}

//fill the _request
// it works as : get the first line based on space
// then check for the ':' however if there is a boundary and its found, keep everything between as body
void Request::parseResponse(const std::string& headers) {
	std::istringstream	iss(headers);
	std::string			line;

	parseFirstLine(iss);
	while (std::getline(iss, line)) {
		if (line == "\r")
			break;
		size_t pos = line.find(':');
		if (pos != std::string::npos)
			_request.emplace_back(create_pair(line, pos));
	}
    if (std::getline(iss, line))
        parseBody(iss, line);
}

void Request::fillBoundary(std::string text){
	std::string search	= "boundary=";
	size_t pos			= text.find(search);

	if (pos == std::string::npos){
		_boundary = "";
		return;
	}
	pos += search.size();
	size_t endPos = text.find("\r\n", pos);
	if (endPos == std::string::npos){
		_boundary = "";
		return;
	}
	while (text[pos] == '-')
		++pos;
    _boundary = text.substr(pos, endPos - pos);
}

//Constructor that parses everything
Request::Request(int clientFd) : _clientFd(clientFd), _doneReading(false), _errorCode(ErrorCode::OK), _errorPageFound(false){}

Request::~Request() {}

void Request::constructRequest(){
	Logger::log("Constructor request call", INFO);
	// std::cout << _request_text << std::endl;

	if (_request_text.empty())
		throw RequestException("Empty request");

	fillBoundary(_request_text);
	parseResponse(_request_text);	
	setFile();

	std::string method = getMethod(0);
	
	Logger::log("Method is :" + method, INFO);
	if (method == "GET"){
		;
	}
	else if (method == "DELETE"){
		handleDelete();
		return;
	}
	else if (method == "POST"){
		handlePost();
		return;
	}
}