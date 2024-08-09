#include "Response.hpp"


Response::Response(int clientFd) : _clientFd(clientFd), _ready(false)
{
	_responseText = "";
}

Response::~Response() {}

std::string getHTTPDate() {
	std::time_t now = std::time(nullptr);
	std::tm tm = *std::gmtime(&now);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
	return oss.str();
}

std::string getLastModified(const std::string& file_path) {
	struct stat file_stat;
	if (stat(file_path.c_str(), &file_stat) == 0) {
		std::tm tm = *std::gmtime(&file_stat.st_mtime);
		std::ostringstream oss;
		oss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
		return oss.str();
	}
	return "Not Found";
}

void Response::addHeaders(Request* request)
{
	std::ostringstream oss;

	std::string http = request->getMethod(2);
	std::string code = std::to_string(request->getErrorCode());
	if (request->getErrorCode() == DIRECTORY_LISTING)
		code = "200";
	
	std::string contentLength = "Content-Length: " + std::to_string(_responseText.size()) + "\r\n";
	std::string date = "Date: " + getHTTPDate() + "\r\n";
	std::string lastModified = "Last-Modified: " + getLastModified(_file) + "\r\n";
	
	std::string message = httpStatus[request->getErrorCode()];

	oss << http << " " << code << " " << message << "\r\n";
	oss << contentLength;
	oss << date;
	oss << lastModified;
	oss << "Connection: close" << "\r\n";
	oss << "\r\n";
	oss << _responseText;
	_responseText = oss.str();
}

void Response::errorResponseHTML(ErrorCode error) {
	std::string text = httpStatus[int(error)];
	_responseText = START_HTML + std::to_string(error) + ", " + text + END_HTML;
}

//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::addToBuffer(std::string buffer)
{
	_responseText += buffer;
}

void Response::rSend()
{
	Logger::log("Sending Response to client...");
	Logger::log("TEXT:\n" + _responseText, LogLevel::DEBUG);
	if (send(_clientFd, _responseText.c_str(), _responseText.length(), 0) == -1)
		Logger::log("Error with sending response", LogLevel::ERROR);
}

void Response::sendRedir()
{
	if (send(_clientFd, _responseText.c_str(), _responseText.length(), 0) == -1)
		Logger::log("Error with send!", LogLevel::ERROR);
}

void Response::setReady(void)
{
	_ready = true;
}

void Response::setResponseString(std::string response)
{
	_responseText = response;
}

int Response::setRedirectStr(int status, std::string from, std::list<s_redirect> redirs)
{
	std::string to = "";
	for (s_redirect r : redirs) {
		if (r.redirFrom == from) {
			to = r.redirTo;
			break;
		}
	}
	if (to == "")
		return (INTERNAL_SRV_ERR);
	std::ostringstream oss;
	oss << "HTTP/1.1 ";
	oss << status;
	oss << " Redirection\r\n";
	oss << "Content-Type: text; charset=utf-8\r\n";
	oss << "Location: ";
	oss << to << "\r\n\r\n";
	oss << REDIR_START << "<a href=\"" << to << "\">" << to << "</a>" << REDIR_END;
	std::cout << oss.str() << std::endl;
	_responseText = oss.str();
	return (status);
}

void Response::setDirectoryListing(Request* request)
{
	std::string	name = request->getConfig().getBasePath() + request->getConfig().getRoot() + request->getFileName();
	if (!exists(name))
		throw (HTTPError(PAGE_NOT_FOUND));
	std::vector<std::string>	v = getDirectoryContent(name.c_str());
	std::ostringstream oss;
	addHeaders(request);
	oss << _responseText;
	oss << DIR_LIST_START;
	oss << name;
	oss << DIR_LIST_MID;
	for (std::string s : v)
	{
		std::string link = name + s;
		if (isDirectory(link.c_str())) {
			s += '/';
		}
		oss << "<li><a href=\"" << s << "\">";
		oss << s << "</a></li>";
	}
	oss << DIR_LIST_END;
	_responseText = oss.str();
}
