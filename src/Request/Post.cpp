#include "Request.hpp"


std::string Request::getOtherFilename() {
	Logger::log(_requestText, WARNING);

	std::size_t filenamePos = _requestText.find("filename=\"");                          // Find the position of the filename in the JSON body
	if (filenamePos == std::string::npos)
		throw HTTPError(BAD_REQUEST); 

	filenamePos += 10;                                                                  // Move past "filename=\"" to get to the start of the actual filename
	std::size_t endQuotePos = _requestText.find("\"", filenamePos);
	if (endQuotePos == std::string::npos) {
		throw HTTPError(BAD_REQUEST);                                                   // Closing quote not found, throw an HTTP error
	}

	return _requestText.substr(filenamePos, endQuotePos - filenamePos);                  // Extract and return the filename
}

void Request::handlePost(const std::string &path, const std::string &file)
{
	// std::string body = getValues("Body");

	// if (body.empty())
	// 	throw (HTTPError(UNPROCESSABLE));

	Logger::log("Creating the file", INFO);
	createFile(_requestText, path, file);
}

void writeBinaryDataToFile(const std::string& path, const std::string& binaryData) {
	std::ofstream file(path, std::ios::binary);

	if (!file) {
		throw std::ios_base::failure("Failed to open file for writing.");
	}

	std::string boundaryIndicator = "\r\n--";
	size_t boundaryPos = binaryData.rfind(boundaryIndicator);

	if (boundaryPos != std::string::npos) {
		file.write(binaryData.data(), boundaryPos);
	} else {
		file.write(binaryData.data(), binaryData.size());
	}

	file.close();
}

//Need to split this function. It's doing way too many things : search extension, append path etcc
void Request::createFile(const std::string &content, std::string path, std::string file){
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

   pos = fileContent.find("\r\n\r\n");
    if (pos == std::string::npos) {
        std::cerr << "Error: No empty line found in content" << std::endl;
        throw HTTPError(BAD_REQUEST);
    }

    fileContent = fileContent.substr(pos + 4); // Skip the two newlines


    //Logger::log("Write: " + fileContent, WARNING);
    try {
        writeBinaryDataToFile(path, fileContent);
        std::cout << "File created successfully." << std::endl;
		throw HTTPError(CREATED);
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Error writing file: " << e.what() << std::endl;
		throw HTTPError(INTERNAL_SRV_ERR);
    }
}