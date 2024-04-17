#include<unistd.h>
#include<fcntl.h>
#include<list>
#include<iostream>
#include"Request.hpp"

Request::Request(){}

static std::list<std::string> makeRequest(std::string str){
	std::list<std::string> request;
	std::string temp;
	int i = 0;
	while (str != "" && i < 30){
		temp = str.substr(0, str.find('\n'));
		request.push_back(temp);
		str.erase(0, temp.length() + 1);
		i++;
	}
	return request;
}

// dont use this one, since read always has to go through poll() first
Request::Request(int fd){

	char buffer[1024];
	std::string request;
	int i = 0;
    while ((i = read(fd, buffer, sizeof(buffer))) > 0){
		buffer[i] = '\0';
		request += buffer;
    }
	_request = makeRequest(request);
	size_t j = request.find_first_of(" ");
	std::string temp = request.substr(0, j);
	for (int k = 0; k <= TRACE; k++){
		if (temp == methodType[k])
			_method = k;
	}
	size_t l = request.find("Host: ") + 6;
	_host = request.substr(l, request.find("\n", l) - l);
	if (_method == POST){
		l = request.find("Content-Type: ");
		_contentType = request.substr((l + 14), request.find("\n", l) - (l + 14));
		l = request.find("Content-Length: ");
		temp = request.substr((l + 16), request.find("\n", l) - (l + 16));
		_size = std::stol(temp);
		temp = _request.back();
		_content = temp.substr(temp.find('=') + 1, temp.length());
	}
}

Request::Request(std::string request){
	_request = makeRequest(request);
	size_t j = request.find_first_of(" ");
	std::string temp = request.substr(0, j);
	for (int k = 0; k < TRACE; k++){
		if (temp == methodType[k])
			_method = k;
	}
	size_t l = request.find("Host: ") + 6;
	size_t m = request.find("\n", l);
	_host = request.substr(l, m - l);
	if (_method == POST){
		l = request.find("Content-Type: ");
		_contentType = request.substr((l + 14), request.find("\n", l) - (l + 14));
		l = request.find("Content-Length: ");
		temp = request.substr((l + 16), request.find("\n", l) - (l + 16));
		_size = std::stol(temp);
		temp = _request.back();
		_content = temp.substr(temp.find('=') + 1, temp.length());
	}
	if (_method == DELETE){
		// do things
	}
}

int	Request::getType() const{
	return _method;
}

std::string	Request::getHost() const{
	return _host;
}

std::string	Request::getContType() const{
	return _contentType;
}

uint32_t	Request::getSize() const{
	return _size;
}

std::string	Request::getContent() const{
	return _content;
}

std::ostream & operator<< (std::ostream &out, const Request& src){
	out << "Request info:\n"
		<< "type:\t" << src.getType() << '\n'
		<< "host:\t" << src.getHost() << std::endl;
	if (src.getType() != GET){
		out << "content type:\t" << src.getContType() << '\n'
			<< "size:\t" << src.getSize() << '\n'
			<< "content:\t" << src.getContent() << std::endl;
	}
	return out;
}

int main(){
	int fd = open("request", O_RDONLY);
	Request test(fd);

	std::cout << test << std::endl;
	return 0;
}