#include "Server.hpp"
#include<fstream>

static std::list<s_port> defaultPorts(){
	std::list<s_port> ports;
	s_port def;
	def.nmb = 80;
	def.type = "default_server";
	s_port ssl;
	ssl.nmb = 443;
	ssl.type = "ssl";
	ports.push_back(def);
	ports.push_back(ssl);
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

static std::list<s_ePage> defaultErrorPages(std::string root){
	std::list<s_ePage> erorPages;
	s_ePage fofo;
	fofo.err = 404;
	fofo.url = root + "errdir/404.html";
	s_ePage fofv;
	fofv.err = 405;
	fofv.url = root + "errdir/405.html";
	erorPages.push_back(fofo);
	erorPages.push_back(fofv);
	return erorPages;
}

static std::list<std::string> defaultIndex(){
	std::list<std::string> index = 
		{"index.php", "index.html", "index.htm"};
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
	std::ifstream infile(conf, std::ios::in);
}

Server::~Server() {
}

Server &Server::operator=(const Server &obj) {
	return *this;
}

Server::Server(const Server &obj) {
	*this = obj;
}

std::list<s_port> Server::getPorts()	const{
	return _ports;
}

std::string Server::getName()	const{
	return _name;
}

std::string Server::getRoot()	const{
	return _root;
}

s_method Server::getMethods()	const{
	return _methods;
}

bool Server::getCGI()	const{
	return _cgi;
}

uint32_t Server::getMaxBody()	const{
	return _maxBody;
}

std::list<s_ePage> Server::getErrorPages()	const{
	return _errorPages;
}

std::list<std::string> Server::getIndex()	const{
	return _index;
}

bool Server::getAutoIndex()	const{
	return _autoIndex;
}

std::string boolstring(const bool& src){
	if (!src)
		return "false";
	else
		return "true";
}

std::ostream & operator<< (std::ostream &out, const Server& src){

	for (s_port port : src.getPorts()){
		out << "port\t" << port.nmb << "\t" << port.type << "\n";
	}
	out << "name\t" << src.getName() << "\n";
	out << "root\t" << src.getRoot() << "\n";
	out << "methods\t";
	if (src.getMethods().GET)
		out << "GET\t";
	if (src.getMethods().POST)
		out << "POST\t";
	if (src.getMethods().DELETE)
		out << "DELETE\t";
	out << "\n";
	out << "cgi\t" << boolstring(src.getCGI()) << "\n";
	out << "max_body\t" << src.getMaxBody() << "bytes" << "\n";
	for (s_ePage ePage : src.getErrorPages()){
		out << "Error\t" << ePage.err << "\t" << ePage.url << "\n";
	}
	for (std::string index : src.getIndex()){
		out << "Index\t" << index << "\n";
	}
	out << "auto Index\t" << boolstring(src.getAutoIndex());
	
	return out;
}
