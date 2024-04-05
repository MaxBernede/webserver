#include "../inc/webserver.hpp"
#include "../inc/utils.hpp"

int genererPort() {
    srand(time(0));
    return rand() % (MAX_PORT - MIN_PORT + 1) + MIN_PORT;
}

std::string read_html_file(const std::string &filename) {
    std::string file_path = "html/" + filename;
    std::ifstream file(file_path);

    if (!file.is_open()){
        std::cout << "Error: Impossible to open the file " << file_path << std::endl;
        return read_html_file("error.html");
    }

    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    oss << file.rdbuf();
    file.close();

    return oss.str();
}

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}