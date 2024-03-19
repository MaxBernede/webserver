#include "Server.hpp"

// Constructors and Destructor
Server::Server() {
	std::cout << "Server default Constructor called" << std::endl;
	this->name = "Server";
}

Server::Server(std::string n) {
	std::cout << "Server Constructor called" << std::endl;
	this->name = n;
}

Server::~Server() {
	std::cout << "Server Destructor called" << std::endl;
}

// Canonical form
Server &Server::operator=(const Server &obj) {
	return *this;
}

Server::Server(const Server &obj) {
	*this = obj;
}
