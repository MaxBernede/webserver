#include "utils.hpp"

//Return the first word after GET (usually the html)
//Check the scalability in case of different parsing ways to do
std::string firstWord(const std::string& str) {
	size_t pos = str.find(' ');

	if (pos == std::string::npos)
		return str;
	return str.substr(0, pos);
}
