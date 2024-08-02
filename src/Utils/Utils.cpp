#include "Utils.hpp"
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

// finds the extension with out the query
std::string getExtension(std::string fileName)
{
	size_t dotIndex = fileName.find_last_of(".");
	if (dotIndex != std::string::npos)
	{
		std::string extension = fileName.substr(dotIndex + 1);
		size_t queryIndex = extension.find('?');
		if (queryIndex != std::string::npos)
		{
			extension = extension.substr(0, queryIndex);
		}
		return extension;
	}
	return "";
}

bool exists (const std::string& name){
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0); 
}

std::string boolstring(const bool& src){
	if (!src)
		return "false";
	else
		return "true";
}

int isDirectory(const char *path)
{
    struct stat path_stat;
   if (stat(path, &path_stat) != 0)
       return 0;
    return S_ISDIR(path_stat.st_mode);
}

std::vector<std::string> getDirectoryContent(const char *name)
{
	std::vector<std::string>	v;
	DIR	*dirp = opendir(name);
    struct dirent	*dp;
    while ((dp = readdir(dirp)) != NULL)
	{
        v.push_back(dp->d_name);
    }
    closedir(dirp);
	return v;
}
