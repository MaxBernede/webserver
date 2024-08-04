#pragma once

#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <filesystem>

#include "ConfigClass.hpp"

std::string	firstWord(const std::string& str);
bool		endsWith(const std::string& str, const std::string& suffix);
bool 		exists (const std::string& name);
void		createFile(std::string const &body, std::string path, std::string file);
std::string	getExtension(std::string fileName);
std::string	getPath();
std::string	boolstring(const bool& src);
int			isDirectory(const char *path);
std::vector<std::string> getDirectoryContent(const char *name);
std::string getFileNameFromBody(std::string const &content);
void		createDirIfNoExist(std::string path) ;