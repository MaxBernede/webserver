#include "Utils.hpp"
#include "Logger.hpp"
//Find filename in the body content
//Find the last " and return whats in between

std::string getFileNameFromBody(std::string const &content){
	std::string	key			= "filename=\"";
	size_t		start		= content.find(key) + key.length();
	std::string	fileName	= content.substr(start, content.find('\"', start) - start);
	return fileName;
}

void createDirIfNoExist(std::string path) {
	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);
}
