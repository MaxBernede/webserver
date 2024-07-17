#include "utils.hpp"
#include <sys/stat.h>

uint32_t configIP(std::string ip){
	uint32_t host = 0;
	while(ip != ""){
		std::string temp = ip.substr(0, ip.find_first_of(".;"));
		if (temp.length() > 3 || temp.length() < 1 || std::stoi(temp) > UINT8_MAX)
			throw syntaxError();
		host = host << 4;
		host += std::stoi(temp);
		ip.erase(0, temp.length() + 1);
	}
	if (host == 0)
		throw syntaxError();
	// std::cout << "IP OK" << std::endl;
	return host;
}

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

std::string getExtension(std::string fileName){
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

void create_file(std::string const &content, std::string const &location){ // not currently working, look into it
	size_t i = 0;
	std::string key = "filename=\"";           
	printColor(GREEN, content);
	printColor(MAGENTA, location);
	size_t start = content.find(key) + key.length();
	std::string fileName = content.substr(start, content.find('\"', start) - start);
    fileName = location + "/" + fileName;
	std::string extension = fileName.substr(fileName.rfind('.'));
	std::string name = fileName.substr(0, fileName.find(extension));
	while (exists(fileName) == true){
		fileName = name + std::to_string(i) + extension;
		i++;
	}
	std::string temp = content;
	std::cout << temp << std::endl;
	// while (temp[0] != '\n'){
	// 	temp.erase(0, (temp.find('\n') + 1));
	// 	printColor(BLUE, "stuck?");}
	temp.erase(0, 1);
	std::ofstream upload(fileName.c_str(), std::ios::out);
	upload << temp;
	upload.close();
}


std::string boolstring(const bool& src){
	if (!src)
		return "false";
	else
		return "true";
}
