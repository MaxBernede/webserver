#pragma once

#include "webserver.hpp"

std::string	firstWord(const std::string& str);
bool		endsWith(const std::string& str, const std::string& suffix);
bool 		exists (const std::string& name);
void		create_file(std::string const &content, std::string const &location);
std::string getExtension(std::string fileName);