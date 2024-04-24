#include "../inc/webserver.hpp"

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

std::list<Server>	init_serv(std::ifstream &conf, char **env){
	std::list<Server> server;
	std::string buf;
	std::list<std::string> block;

	int i = 1;

	while (!conf.eof()){
		std::getline(conf, buf);
		if (servBlockStart(buf)){
			try{
				while (true){
					std::getline(conf, buf);
					if (buf.empty())
						continue ;
					if (buf.front() == '}')
						break;
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
				server.push_front(def);
			}
		}
	}
	return server;
}

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
	if (!env[i])
		return ("/var/www/");
	std::string test = (env[i] + 4);
	return test + '/';
}

static std::vector<bool> defaultMethods(){
	std::vector<bool> methods;
	for (int i = GET; i <= TRACE; i++){
		if (i == GET || i == POST || i == DELETE)
			methods.push_back(true);
		else
			methods.push_back(false);
	}
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

//default consructor
Server::Server(char **env):
	_ports(defaultPorts()),
	_name(defaultName()),
	_root(defaultRoot(env)),
	_methods(defaultMethods()),
	_cgi(true),
	_maxBody(1048576),
	_errorPages(defaultErrorPages(_root)),
	_index(defaultIndex()),
	_autoIndex(true){
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

bool Server::getMethod(int i)	const{
	return _methods[i];
}

std::vector<bool> Server::getMethods()	const{
	return _methods;
}

bool Server::getCGI()	const{
	return _cgi;
}

uint64_t Server::getMaxBody()	const{
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

void Server::clearPort(){
	_ports.clear();
}

void Server::clearName(){
	// _name = "";
}

void Server::clearRoot(){
	// _root = "";
}

void Server::clearMethods(){
	for (int i = GET; i <= TRACE; i++)
		_methods[i] = false;
}

void Server::clearCGI(){
	// _cgi = false;
}

void Server::clearMaxBody(){
	// _maxBody = 200;
}

void Server::clearEPage(){
	_errorPages.clear();
}

void Server::clearIndex(){
	_index.clear();
}

void Server::clearAutoIndex(){
	// _autoIndex = false;
}

void Server::clearData(int index){
	void (Server::*ptr[9])(void) = 
		{&Server::clearPort, &Server::clearName, &Server::clearRoot, &Server::clearMethods, &Server::clearCGI,
		&Server::clearMaxBody, &Server::clearEPage, &Server::clearIndex, &Server::clearAutoIndex};
	(this->*ptr[index])();
}

void Server::setPort(s_port port){
	_ports.push_back(port);
}

void Server::setName(std::string name){
	_name = name;
}

void Server::setRoot(std::string root){
	_root = root;
}

void Server::setMethod(int method, bool value){
	_methods[method] = value;
}

void Server::setCGI(bool CGI){
	_cgi = CGI;
}

void Server::setMaxBody(uint64_t body){
	_maxBody = body;
}

void Server::setErrorPages(s_ePage ePage){
	_errorPages.push_back(ePage);	
}

void Server::setIndex(std::string index){
	_index.push_back(index);
}

void Server::setAutoIndex(bool autoIndex){
	_autoIndex = autoIndex;
}

std::string boolstring(const bool& src){
	if (!src)
		return "false";
	else
		return "true";
}

std::ostream & operator<< (std::ostream &out, const Server& src){

	for (s_port port : src.getPorts()){
		out << "port\t" << port.nmb << "\t" << port.type << std::endl;
	}
	out << "name\t" << src.getName() << std::endl;
	out << "root\t" << src.getRoot() << std::endl;
	out << "methods\t";
	for (int i = GET; i <= TRACE; i++){
		out << boolstring(src.getMethod(i)) << "\t";
	}
	out << std::endl;
	out << "cgi\t" << boolstring(src.getCGI()) << std::endl;
	out << "max_body\t" << src.getMaxBody() << " bytes" << std::endl;
	for (s_ePage ePage : src.getErrorPages()){
		out << "Error\t" << ePage.err << "\t" << ePage.url << std::endl;
	}
	for (std::string index : src.getIndex()){
		out << "Index\t" << index << std::endl;
	}
	out << "auto Index\t" << boolstring(src.getAutoIndex()) << std::endl;
	
	return out;
}
