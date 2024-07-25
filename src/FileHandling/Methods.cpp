#include "FileHandling.hpp"

bool containsSubstring(const std::string& subStr, const std::string& mainStr) {
	// Logger::log(mainStr, INFO);
	// Logger::log(subStr, INFO);
    return mainStr.find(subStr) != std::string::npos;
}

bool verifyPath(std::string &inputPath) {
    bool endsWithSlash = !inputPath.empty() && inputPath.back() == '/';

    try {
        std::filesystem::path pathObj(inputPath);
        std::filesystem::path absolute_path = std::filesystem::weakly_canonical(pathObj);
        inputPath = absolute_path.string();

        if (endsWithSlash)
            inputPath += '/';

		if (!containsSubstring(getPath() + "/saved_files/", inputPath))
			return false;
		return true;
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        return false;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return false;
    }
}
