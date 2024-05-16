#include "../inc/webserver.hpp"
#include "../inc/utils.hpp"

//generate random port between 8000 and 8100
// because otherwise need to change manually every run
int genererPort() {
    srand(time(0));
    return rand() % (MAX_PORT - MIN_PORT + 1) + MIN_PORT;
}

//Try to read an html file and return it as a string with a specific text for response
//if the file doesnt end with .html or is not found/open, this func will call 
//itself again with error.html

std::string make_response(std::ifstream &file){
    std::ostringstream oss;
    oss << "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\n\r\n";
    oss << file.rdbuf();
    file.close();

    return oss.str();
}

std::string read_html_file(const std::string &filename) {
    std::string file_path = "html/" + filename;
    std::ifstream file(file_path);

    if (!file.is_open()){
        std::cout << "Error: Impossible to open the file " << file_path << std::endl;
        return "";
    }

    return make_response(file);

}

//Check if the first string end with the second
bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

void create_file(std::string const &content, std::string const &location){
	size_t i = 0;
	std::string key = "filename=\"";
	size_t start = content.find(key) + key.length();
	std::string fileName = content.substr(start, content.find('\"', start) - start);
    fileName = location + "/" + fileName;
	std::string extension = fileName.substr(fileName.rfind('.'));
	std::string name = fileName.substr(0, fileName.find(extension));
	while (exists(fileName) == true){
		fileName = name + std::to_string(i) + extension;
		i++;
	}
	std::string temp = content;
	while (temp[0] != '\n')
		temp.erase(0, (temp.find('\n') + 1));
	temp.erase(0, 1);
	std::ofstream upload(fileName.c_str(), std::ios::out);
	upload << temp;
	upload.close();
}

//Return the first word after GET (usually the html)
//Check the scalability in case of different parsing ways to do
std::string firstWord(const std::string& str) {
	size_t pos = str.find(' ');

	if (pos == std::string::npos)
		return str;
	return str.substr(0, pos);
}