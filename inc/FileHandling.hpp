#pragma once

#include <filesystem>
#include <string>
#include "Utils.hpp"
#include <iostream>
#include <sys/stat.h>

#include "HTTPError.hpp"

bool 		verifyPath(std::string& inputPath);
bool		isFile(std::string filePath);