#pragma once

#include <string>
#include "ConfigClass.hpp"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

std::string	firstWord(const std::string& str);
bool		endsWith(const std::string& str, const std::string& suffix);
bool 		exists (const std::string& name);
void		createFile(std::string const &content, std::string const &location);
std::string	getExtension(std::string fileName);
std::string	getPath();
std::string	boolstring(const bool& src);
int			isDirectory(const char *path);
