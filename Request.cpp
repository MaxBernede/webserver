#include"Request.hpp"
#include<unistd.h>
#include<fcntl.h>

Request::Request(){}

Request::Request(int fd){
	char buffer[1024];
	std::string request;
	int i = 0;
    while ((i = read(fd, buffer, sizeof(buffer))) > 0){
		buffer[i] = '\0';
		request += buffer;
    }
	_request = request;
	size_t j = request.find_first_of(" ");
	std::string temp = request.substr(0, j);
	for (int k = 0; k <= TRACE; k++){
		if (temp == methodType[k])
			_type = k;
	}
	size_t l = request.find("Host: ") + 6;
	size_t m = request.find("\n", l);
	_host = request.substr(l, m - l);
}

Request::Request(std::string request){
	_request = request;
	size_t j = request.find_first_of(" ");
	std::string temp = request.substr(0, j);
	for (int k = 0; k < TRACE; k++){
		if (temp == methodType[k])
			_type = k;
	}
	size_t l = request.find("Host: ") + 6;
	size_t m = request.find("\n", l);
	_host = request.substr(l, m - l);
}

int	Request::getType() const{
	return _type;
}

std::string	Request::getHost() const{
	return _host;
}

std::ostream & operator<< (std::ostream &out, const Request& src){
	out << "Request info:\n"
		<< "type:\t" << src.getType() << "\n"
		<< "host:\t" << src.getHost() << std::endl;
	return out;
}

int main(){
	int fd = open("test", O_RDONLY);
	Request test(fd);

	std::cout << test << std::endl;
	return 0;
}