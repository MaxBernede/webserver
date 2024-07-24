#pragma once

#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <csignal>

std::string	firstWord(const std::string& str);
bool		endsWith(const std::string& str, const std::string& suffix);
bool 		exists (const std::string& name);
void		createFile(std::string const &content, std::string const &location);
std::string getExtension(std::string fileName);
std::string getPath();
uint32_t configIP(std::string ip);
std::string boolstring(const bool& src);