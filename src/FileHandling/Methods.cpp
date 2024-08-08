#include "FileHandling.hpp"

bool containsSubstring(const std::string& subStr, const std::string& mainStr) {
	return mainStr.find(subStr) != std::string::npos;
}

bool isFile(std::string filePath){
	struct stat path_stat;
	int statRes = stat(filePath.c_str(), &path_stat);
	if (statRes != 0)
		throw HTTPError(INTERNAL_SRV_ERR);
	return (S_ISREG(path_stat.st_mode));
}
