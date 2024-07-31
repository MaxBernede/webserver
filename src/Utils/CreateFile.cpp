#include "Utils.hpp"
//Find filename in the body content
//Find the last " and return whats in between

std::string getFileNameFromBody(std::string const &content){
	std::string	key			= "filename=\"";
	size_t		start		= content.find(key) + key.length();
	std::string	fileName	= content.substr(start, content.find('\"', start) - start);
	return fileName;
}

//Need to split this function. It's doing way too many things : search extension, append path etcc
void createFile(std::string const &content, std::string path, std::string file){
	size_t i				= 1;
	std::string extension	= getExtension(file); 				//	c, html or anything after the '.'
	std::string name		= file.substr(0, file.find("." + extension)); // ft_strrchr
	std::string temp		= content;

	path += file;
	// Remove everything until the empty line then we have all the content
	while (temp[0] != '\n')
		temp.erase(0, (temp.find('\n') + 1));
	temp.erase(0, 1);
	std::ofstream upload(path.c_str(), std::ios::out);
	upload << temp;
	upload.close();
}

void createDirIfNoExist(std::string path) {
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directories(path);
	}
}
