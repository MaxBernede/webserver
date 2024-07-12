#include "../inc/webserver.hpp"
#include "../inc/utils.hpp"

//Find filename in the body content
//Find the last " and return whats in between
std::string getFileName(std::string const &content){
	std::string key = "filename=\"";
	size_t start = content.find(key) + key.length();
	std::string fileName = content.substr(start, content.find('\"', start) - start);
	return fileName;
}

//Need to split this function. It's doing way too many things : search extension, append path etcc
void create_file(std::string const &content, std::string const &location){
	size_t i				= 0;
	std::string fileName	= getFileName(content);   //	ft_strrchr.c for exemple
	std::string extension	= getExtension(fileName); //	c, html or anything after the '.'
	std::string name		= fileName.substr(0, fileName.find("." + extension)); // ft_strrchr
	std::string path		= location + "/" + fileName;
	//printColor(MAGENTA, name, " ", path, " ",extension, " ",fileName);

	while (exists(path) == true){
		path = name + std::to_string(i) + extension;
		i++;
	}
	std::string temp = content;
	while (temp[0] != '\n')
		temp.erase(0, (temp.find('\n') + 1));
	temp.erase(0, 1);
	std::ofstream upload(path.c_str(), std::ios::out);
	upload << temp;
	upload.close();
}