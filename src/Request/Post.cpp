#include "Request.hpp"


void Request::handlePost(std::string path, std::string file)
{
	std::string body = getValues("Body");

	if (body.empty())
		throw (HTTPError(OK)); // check if not 422

	Logger::log("Creating the file", INFO);
	createFile(body, path, file);
}

void writeBinaryDataToFile(const std::string& path, const std::string& binaryData) {
    std::ofstream file(path, std::ios::out | std::ios::binary);

    if (!file) {
        throw std::ios_base::failure("Failed to open file for writing.");
    }
	
    file.write(binaryData.data(), binaryData.size());

    file.close();
}

// Skip lines until the next empty line
std::string skipUntilEmptyLine(std::string content) {
    size_t pos;
    while ((pos = content.find('\n')) != std::string::npos) {
        if (content.substr(0, pos) == "") {
            content.erase(0, pos + 1); // Skip the empty line itself
            break;
        }
        content.erase(0, pos + 1);
    }
    return content;
}

//Need to split this function. It's doing way too many things : search extension, append path etcc
void Request::createFile(std::string const &content, std::string path, std::string file){
	std::string extension	= getExtension(file); 				//	c, html or anything after the '.'
	std::string name		= file.substr(0, file.find("." + extension)); // ft_strrchr

	path += file;

	// Just for some security
	if (file.empty())
		throw HTTPError(BAD_REQUEST);

    // Find the empty lines
    size_t pos = content.find("\r\n\r\n");
    if (pos == std::string::npos) {
        std::cerr << "Error: No empty line found in content" << std::endl;
        throw HTTPError(BAD_REQUEST);
    }

    std::string fileContent = content.substr(pos + 4); // Skip the two newlines


    //Logger::log("Write: " + fileContent, WARNING);
    try {
        writeBinaryDataToFile(path, fileContent);
        std::cout << "File written successfully." << std::endl;
		throw HTTPError(NO_CONTENT);
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Error writing file: " << e.what() << std::endl;
		throw HTTPError(INTERNAL_SRV_ERR);
    }
}