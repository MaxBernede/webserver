#include "Server.hpp"

static s_port* defaultPorts(){
	s_port *ports = new s_port[2];
	ports[0].nmb = 80;
	ports[0].type = "default_server";
	ports[1].nmb = 443;
	ports[1].type = "ssl";
	return ports;
}

static std::string defaultName(){
	return "webserv.com";
}

static bool compare(std::string s1, std::string s2){

	if (s1.compare(0, 4, s2) == 0)
		return true;
	return false;
}
// TODO check if pwd even exist/find alternative?
static std::string defaultRoot(char **env){
	int i = 0;
	while (env[i] && !compare(env[i], "PWD=")){
		i++;
	}
	return env[i] + 4;
}

static s_method defaultMethods(){
	s_method methods;
	methods.GET = true;
	methods.POST = true;
	methods.DELETE = true;
	return methods;
}

static s_ePage *defaultErrorPages(std::string root){
	s_ePage* errorPages = new s_ePage[2];
	errorPages[0].err = 404;
	errorPages[0].url = root + "errdir/404.html";
	errorPages[1].err = 405;
	errorPages[1].url = root + "errdir/405.html";
	return errorPages;
}

static std::string* defaultIndex(){
	std::string* index = new std::string[3];
	index[0] = "index.php";
	index[1] = "index.html";
	index[2] = "index.htm";
	return index;
}

//default consructor, called when conf could not be read / was formatted improperly
Server::Server(char **env):
	_ports(defaultPorts()),
	_name(defaultName()),
	_root(defaultRoot(env)),
	_methods(defaultMethods()),
	_cgi(true),
	_maxBody(1048576),
	_errorPages(defaultErrorPages(_root)),
	_index(defaultIndex()),
	_autoIndex(true)
{}
//TODO read from file and do stuff / habdle multiple server blocks?
Server::Server(char* conf, char **env) {
	// this->_name = n;
}

Server::~Server() {
	delete []_ports;
	delete []_errorPages;
	delete []_index;
}

Server &Server::operator=(const Server &obj) {
	return *this;
}

Server::Server(const Server &obj) {
	*this = obj;
}
