#include "../inc/webserver.hpp"
#include "../inc/response.hpp"

#define BUFFER_SIZE 10024

void removeTrailingCarriageReturn(std::string& str) {
    if (!str.empty() && str.back() == '\r')
        str.pop_back();
}

std::string substringAfter(const std::string& str, const std::string& delimiter) {
    size_t pos = str.find(delimiter);
    if (pos != std::string::npos)
        return str.substr(pos + delimiter.length());
    return ""; // Return empty string if delimiter not found
}

void Response::do_actions(request request){
	std::string file = request.get_file();
	if (endsWith(file, ".html")){
		std::cout << "FILE END WITH HTML" << std::endl;
	}
	if (request.get_method(0) == "POST" && request.get_file() == "/delete_post"){
		std::cout << "RESPONSE IS DELETING THE FILE" << std::endl;
		std::string tmp = request.get_values("Referer");
		file = substringAfter(tmp, "http://localhost:8080/");
		removeTrailingCarriageReturn(file);
		if (file.empty())
			file = "index.html";
	}
	response_text = read_html_file(file);
}


//Read from the FD and fill the buffer with a max of 1024, then get the html out of it
//read the HTML and return it as a string
void Response::handle_request() {
	std::string request_data;
	char buffer[BUFFER_SIZE];
	int bytes_read;

	while (true){
		if ((bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1)) < 0){
			std::cerr << "Error reading request" << std::endl;
			return;
		}
		printColor(RED, bytes_read);
		buffer[bytes_read] = '\0'; 
		if (bytes_read < BUFFER_SIZE || bytes_read == 0){
			request_data += buffer;
			break;
		}
		request_data += buffer;
	}
	

    // while (true) {
    //     int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
    //     if (bytes_read < 0) {
    //         std::cerr << "Error reading request" << std::endl;
    //         return;
    //     }
	// 	else if (bytes_read == 0)
    //         break;
    //     buffer[bytes_read] = '\0'; 
    //     request_data += buffer;
    // }





	request request(request_data);

	html_file = request.get_html();
	do_actions(request);
	r_send();
}

void Response::r_send(){
	//std::cout << "Message to send : " << response_text << std::endl;
	if (send(client_fd, response_text.c_str(), response_text.length(), 0) == -1) {
		std::cerr << "Error sending response" << std::endl;
	}
}

//!Constructors
Response::Response(){
	std::cout << "Default constructor Response" << std::endl;
}
Response::Response(int fd){
	//std::cout << "Constructor Response" << std::endl;
	client_fd = fd;
}

//!Getters
int Response::get_client_fd(){
	return client_fd;
}