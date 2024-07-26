#include "Webserver.hpp"
#include "Logger.hpp"

// add check for empty line
bool servBlockStart(std::string buf){
	if (buf.empty())
		return false;
	if (buf.back() != '{')
		return false;
	if (buf.find("server") != 0)
		return false;
	for (size_t i = 6; i < (buf.size() - 1); i++){
		if (buf[i] != 32 && !(buf[i] >= 9 && buf[i] <= 13))
			return false;
	}
	return true;
}

static std::string findKey(std::string str){
	if (str.find_first_of("\t\n\v\f\r ") != std::string::npos)
		return str.substr(0, str.find_first_of("\t\n\v\f\r "));
	else
		return str.substr(0, (str.length() - 1));
}

Server	pushBlock(std::list<std::string> block, char **env){
	Server serv(env);
	void (*ptr[11])(std::string, Server&) = {&confPort, &confName, &confRoot, &confMethods, &confCGI,
		&confMaxBody, &confErrorPage, &confIndex, &confAutoIndex, &confRedirect, &confPath};
	std::string const keys[11] = {"listen", "serverName", "root", "allowedMethods", "cgiAllowed",
			"clientMaxBodySize", "errorPage", "index", "autoIndex", "return", "path"};
	bool clear[11] = {false, false, false, false, false,
		false, false, false, false, false, false};
	std::list<std::string>::iterator it = block.begin();
	while (it != block.end()){
		std::string str = *it;
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		if (str.front() == '#'){
			it++;
			continue;
		}
		if (str.find("location ") == 0 && str.back() == '{'){
			std::list<std::string> body;
			while (true){
				str = *it;
				while (str.find_first_of("\t\n\v\f\r ") == 0)
					str.erase(0, 1);
				body.push_back(str);
				it++;
				if (str.front() == '}' && str.length() == 1)
					break ;
				else if (it == block.end())
					throw syntaxError();
			}
			addLocation(body, serv);
			continue;
		}
		else {
			if (str.back() != ';')
				throw syntaxError();
			try{
				std::string key = findKey(str);
				str.erase(0, key.length());
				while (str.find_first_of("\t\n\v\f\r ") == 0)
					str.erase(0, 1);
				for (int i = 0; i < 11; i++){
					if (key == keys[i]){
						if (clear[i] == false){
							serv.clearData(i);
							clear[i] = true;
						}
						(*ptr[i])(str, serv);
						break;
					}
					if (i == 10)
						throw syntaxError();
				}
			}
			catch(std::exception const &e){
				std::cout << e.what() << std::endl;
				continue;
			}
		}
		it++;
	}
	serv.setPath(serv.getRoot() + serv.getPath());
	serv.configLocation();
	// std::cout << "SERVER LOCATION OK" << std::endl;
	return serv;
}

std::list<Server>	init_serv(std::ifstream &conf, char **env){
	std::list<Server> server;
	std::string buf;
	std::list<std::string> block;
	bool location = false;

	while (!conf.eof()){
		std::getline(conf, buf);
		if (servBlockStart(buf)){
			try{
				while (true){
					std::getline(conf, buf);
					if (buf.empty())
						continue ;
					if (buf.find("location ") == 0 && buf.back() == '{'){
						if (location == false)
							location = true;
						else
							throw syntaxError();
					}
					if (buf.front() == '}' && buf.length() == 1){
						if (location == false)
							break;
						else
							location = false;
					}
					if (conf.eof())
						throw syntaxError();
					block.push_back(buf);
				}
				server.push_back(pushBlock(block, env));
				block.clear();
			}
			catch(std::exception &e){
				std::cout << e.what() << std::endl;
				Server def(env);
				def.setPath(def.getRoot());
				server.push_front(def);
			}
		}
	}
	// std::cout << "SERVER LIST OK" << std::endl;
	return server;
}

static std::list<s_domain> defaultPorts(){
	std::list<s_domain> ports;
	s_domain def;
	def.port = 8080;
	def.host = "127.0.0.1";
	s_domain ssl;
	ssl.port = 8000;
	ssl.host = "127.0.0.1";
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

//! check the function I've built for that
static std::string defaultRoot(char **env){
	int i = 0;
	while (env != nullptr && env[i] && !compare(env[i], "PWD=")){
		i++;
	}
	if (!env || !env[i])
		return ("/var/www/");
	std::string test = (env[i] + 4);
	return test + '/';
}

static std::list<s_ePage> defaultErrorPages(){
	std::list<s_ePage> erorPages;
	s_ePage fofo;
	fofo.err = 404;
	fofo.url = "errdir/404.html";
	s_ePage fofv;
	fofv.err = 405;
	fofv.url = "errdir/405.html";
	erorPages.push_back(fofo);
	erorPages.push_back(fofv);
	return erorPages;
}

//default constructor
Server::Server(char **env):
	Config(),
	_ports(defaultPorts()),
	_name(defaultName()),
	_root(_basePath + "html/"),
	_maxBody(1048576),
	_errorPages(defaultErrorPages()){
	
	Logger::log("Root is : " + _root);
	_path = "";
}

Server::Server():
	_ports(defaultPorts()),
	_name(defaultName()),
	_root(_basePath + "html/"),
	_maxBody(1048576),
	_errorPages(defaultErrorPages()){
	_path = "";
}

Server::~Server() {
}

Server &Server::operator=(const Server &obj) {
	this->_ports = obj.getPorts();
	this->_name = obj.getName();
	this->_root = obj.getRoot();
	this->_methods = obj.getMethods();
	this->_cgi = obj.getCGI();
	this->_maxBody = obj.getMaxBody();
	this->_errorPages = obj.getErrorPages();
	this->_index = obj.getIndex();
	this->_autoIndex = obj.getAutoIndex();
	this->_redirect = obj.getRedirect();
	this->_location = obj.getLocation();
	this->_path = obj.getPath();
	return *this;
}

Server::Server(const Server &obj) : Config(obj){
	*this = obj;
}

std::list<s_domain> Server::getPorts()	const{
	return _ports;
}

std::string Server::getName()	const{
	return _name;
}

std::string Server::getRoot()	const{
	return _root;
}

uint64_t Server::getMaxBody()	const{
	return _maxBody;
}

std::list<s_ePage> Server::getErrorPages()	const{
	return _errorPages;
}

std::list<Location> Server::getLocation() const{
	return _location;
}

void Server::clearPort(){
	_ports.clear();
}

void Server::clearName(){
}

void Server::clearRoot(){
}

void Server::clearMethods(){
	for (int i = GET; i <= HEAD; i++)
		_methods[i] = false;
}

void Server::clearCGI(){
}

void Server::clearMaxBody(){
}

void Server::clearEPage(){
	_errorPages.clear();
}

void Server::clearIndex(){
	_index.clear();
}

void Server::clearAutoIndex(){
}

void Server::clearRedirect(){
}

void Server::clearPath(){
}

void Server::clearData(int index){
	void (Server::*ptr[11])(void) = 
		{&Server::clearPort, &Server::clearName, &Server::clearRoot, &Server::clearMethods, &Server::clearCGI, &Server::clearMaxBody,
			&Server::clearEPage, &Server::clearIndex, &Server::clearAutoIndex, &Server::clearRedirect, &Server::clearPath};
	(this->*ptr[index])();
}

void Server::setPort(s_domain port){
	_ports.push_back(port);
}

void Server::setName(std::string name){
	_name = name;
}

void Server::setRoot(std::string root){
	_root = root;
}

void Server::setMaxBody(uint64_t body){
	_maxBody = body;
}

void Server::setErrorPages(s_ePage ePage){
	_errorPages.push_back(ePage);	
}

void Server::setLocation(Location &location){
	_location.push_back(location);
}

void Server::configLocation(){
	std::list<Location>::iterator it = _location.begin();
	while (it != _location.end()){
		(*it).autoConfig(*this);
		it++;	
	}
}

std::ostream & operator<< (std::ostream &out, const Server& src){

	for (s_domain port : src.getPorts()){
		out << "port\t" << port.host << "\t" << port.port << std::endl;
	}
	out << "name\t" << src.getName() << std::endl;
	out << "root\t" << src.getRoot() << std::endl;
	out << "methods\t";
	for (int i = GET; i <= HEAD; i++){
		out << boolstring(src.getMethod(i)) << "\t";
	}
	out << std::endl;
	out << "cgi\t" << boolstring(src.getCGI()) << std::endl;
	out << "max_body\t" << src.getMaxBody() << " bytes" << std::endl;
	for (s_ePage ePage : src.getErrorPages()){
		out << "Error\t" << ePage.err << "\t" << ePage.url << std::endl;
	}
	out << "Index\t" << src.getIndex() << std::endl;
	out << "auto Index\t" << boolstring(src.getAutoIndex()) << std::endl;
	for (s_redirect redir : src.getRedirect()){
		out << "Value\t" << redir.returnValue << "\tFROM " << redir.redirFrom 
			<< "\tTO " << redir.redirTo << std::endl;
	}
	out << "Path\t" << src.getPath() << std::endl;
	for (Location l : src.getLocation()){
		out << l << std::endl;
	}
	return out;
}
