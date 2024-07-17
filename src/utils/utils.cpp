#include "utils.hpp"
#include <sys/stat.h>

//Return the first word after GET (usually the html)
//Check the scalability in case of different parsing ways to do
std::string firstWord(const std::string& str) {
	size_t pos = str.find(' ');

	if (pos == std::string::npos)
		return str;
	return str.substr(0, pos);
}

//Check if the first string end with the second
bool endsWith(const std::string& str, const std::string& suffix) {
	if (str.length() < suffix.length()) {
		return false;
	}
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string getExtension(std::string fileName) {
	size_t dotIndex = fileName.find_last_of(".");

	if (dotIndex != std::string::npos)
	{
		std::string extension = fileName.substr(dotIndex + 1);
		return (extension);
	}
	return ("");
}

bool exists (const std::string& name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0); 
}