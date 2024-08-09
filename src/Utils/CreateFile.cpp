#include "Utils.hpp"
#include "Logger.hpp"
#include "HTTPError.hpp"

void createDirIfNoExist(std::string path) {
	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);
}
