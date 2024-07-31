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
    // Open file in binary mode
    std::ofstream file(path, std::ios::out | std::ios::binary);

    // Check if the file was opened successfully
    if (!file) {
        throw std::ios_base::failure("Failed to open file for writing.");
    }

    // Write the binary data to the file
    file.write(binaryData.data(), binaryData.size());

    // Close the file
    file.close();
}


//Need to split this function. It's doing way too many things : search extension, append path etcc
void Request::createFile(std::string const &content, std::string path, std::string file){
	size_t i				= 1;
	std::string extension	= getExtension(file); 				//	c, html or anything after the '.'
	std::string name		= file.substr(0, file.find("." + extension)); // ft_strrchr
	std::string temp		= getValues("Body");

	path += file;
	// Remove everything until the empty line then we have all the content
	while (temp[0] != '\n')
		temp.erase(0, (temp.find('\n') + 1));
	temp.erase(0, 1);
	try {
			writeBinaryDataToFile(path, temp);
			std::cout << "File written successfully." << std::endl;
		} catch (const std::ios_base::failure& e) {
			std::cerr << "Error writing file: " << e.what() << std::endl;
		}
}