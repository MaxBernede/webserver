#include <webserver.hpp>

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
	def.port = 8080;
	def.host = configIP("127.0.0.1");
	s_port ssl;
	ssl.port = 8000;
	ssl.host = configIP("127.0.0.1");
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
	while (env != nullptr && env[i] && !compare(env[i], "PWD=")){
		i++;
	}
	if (!env || !env[i])
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

static std::string defaultIndex(){
	return "";
}

static std::list<s_redirect> defaultRedirect(){
	std::list<s_redirect> redirect;
	redirect.clear();
	return redirect;
}

//default constructor
Server::Server(char **env):
	_ports(defaultPorts()),
	_name(defaultName()),
	_root(defaultRoot(env)),
	_methods(defaultMethods()),
	_cgi(true),
	_maxBody(1048576),
	_errorPages(defaultErrorPages()),
	_index(defaultIndex()),
	_autoIndex(true),
	_redirect(defaultRedirect()){
}

Server::Server():
	_ports(defaultPorts()),
	_name(defaultName()),
	_root(defaultRoot(nullptr)),
	_methods(defaultMethods()),
	_cgi(true),
	_maxBody(1048576),
	_errorPages(defaultErrorPages()),
	_index(defaultIndex()),
	_autoIndex(true),
	_redirect(defaultRedirect()){
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

std::string Server::getIndex()	const{
	return _index;
}

bool Server::getAutoIndex()	const{
	return _autoIndex;
}

std::list<s_redirect> Server::getRedirect() const{
	return _redirect;
}

void Server::clearPort(){
	_ports.clear();
}

void Server::clearName(){
}

void Server::clearRoot(){
}

void Server::clearMethods(){
	for (int i = GET; i <= TRACE; i++)
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

void Server::clearData(int index){
	void (Server::*ptr[10])(void) = 
		{&Server::clearPort, &Server::clearName, &Server::clearRoot, &Server::clearMethods, &Server::clearCGI,
		&Server::clearMaxBody, &Server::clearEPage, &Server::clearIndex, &Server::clearAutoIndex, &Server::clearRedirect};
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
	_index = index;
}

void Server::setAutoIndex(bool autoIndex){
	_autoIndex = autoIndex;
}

void Server::setRedirect(s_redirect redir){
	_redirect.push_back(redir);
}

std::ostream & operator<< (std::ostream &out, const Server& src){

	for (s_port port : src.getPorts()){
		out << "port\t" << port.host << "\t" << port.port << std::endl;
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
	out << "Index\t" << src.getIndex() << std::endl;
	out << "auto Index\t" << boolstring(src.getAutoIndex()) << std::endl;
	for (s_redirect redir : src.getRedirect()){
		out << "Value\t" << redir.returnValue << "\tFROM " << redir.redirFrom 
			<< "\tTO " << redir.redirTo << std::endl;
	}	
	return out;
}
