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