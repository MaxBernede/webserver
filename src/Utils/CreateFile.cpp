#include "Utils.hpp"
#include "Logger.hpp"
#include "HTTPError.hpp"
//Find filename in the body content
//Find the last " and return whats in between

std::string getFileNameFromBody(std::string const &content){
	Logger::log(content, WARNING);
	std::string key = "filename=\"";
	size_t key_pos = content.find(key);

	if (key_pos == std::string::npos) {
		throw HTTPError(BAD_REQUEST);
	}

	size_t start = key_pos + key.length();
	size_t end = content.find('\"', start);

	if (end == std::string::npos) {
		throw HTTPError(BAD_REQUEST);
	}

	std::string fileName = content.substr(start, end - start);
	return fileName;

}

void createDirIfNoExist(std::string path) {
	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);
}
