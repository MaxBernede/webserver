#include "Location.hpp"
#include "ConfigClass.hpp"
#include "Webserver.hpp"
Location::Location(std::list<std::string> &body){
	for (std::string s : body)
		_body.push_back(s);
}

Location::~Location(){}

Location::Location(const Location &obj) :Config(obj), _body(obj._body){
	*this = obj;
}

Location &Location::operator=(const Location &obj){
	this->_name = obj.getName();
	this->_methods = obj.getMethods();
	this->_redirect = obj.getRedirect();
	this->_root = obj.getRoot();
	this->_autoIndex = obj.getAutoIndex();
	this->_index = obj.getIndex();
	this->_cgi = obj.getCGI();
	this->_path = obj.getPath();
	return *this;
}

std::string	Location::getName() const{
	return _name;
}

std::string	Location::getRoot() const{
	return _root;
}

void	Location::setName(std::string name){
	_name = name;
}

void	Location::setMethod(int method, bool value){
	_methods[method] = value;
}

void	Location::setRoot(std::string root){
	_root = root;
}

static std::string	extractName(std::string const &src){
	std::string name = src;
	name.erase(0, 9);
	name.erase(name.length() - 2, 2);
	if (name.length() < 2)
		throw syntaxError();
	return name;
	// std::cout << "NAME LOCATION OK" << std::endl;
}

void	confMethods(std::string value, Location &loc){
	std::string const meth[8] = {"GET", "POST", "DELETE", "PUT",
		"PATCH", "CONNECT", "OPTIONS", "TRACE"};
	for (int i = 0; i < 8; i++){
		loc.setMethod(i, false);
	}
	while (value != ";"){
		std::string tmp = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		for (int i = 0; i < 8; i++)
		{
			if (tmp == meth[i]){
				loc.setMethod(i, true);
				break;
			}
			if (i == 7)
				throw syntaxError();
		}
		value.erase(0, tmp.length());
		while (value.find_first_of("\t\n\v\f\r ") == 0)
			value.erase(0, 1);
	}
	// std::cout << "METHODS LOCATION OK" << std::endl;
}

void	confRedirect(std::string value, Location &loc){
	std::string num = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
	if (num.length() != 3 || num.find_first_not_of("1234567890") != std::string::npos){
		throw syntaxError();}
	value.erase(0, num.length());
	while (value.find_first_of("\t\n\v\f\r ") == 0)
		value.erase(0, 1);
	std::string from = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
	if (from.length() == 0){
		throw syntaxError();}
	value.erase(0, from.length());
	while (value.find_first_of("\t\n\v\f\r ") == 0)
		value.erase(0, 1);
	std::string to = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
	if (to.length() == 0){
		throw syntaxError();}
	value.erase(0, to.length());
	while (value.find_first_of("\t\n\v\f\r ") == 0)
		value.erase(0, 1);
	if (value != ";"){
		throw syntaxError();}
	s_redirect redir;
	redir.returnValue = std::stoi(num);
	redir.redirFrom = from;
	redir.redirTo = to;
	// std::cout << "REDIR LOCATION OK" << std::endl;
	loc.setRedirect(redir);
}

void	confRoot(std::string value, Location &loc){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	// std::cout << "ROOT LOCATION OK" << std::endl;
	loc.setRoot(value);
}

void	confAutoIndex(std::string value, Location &loc){
	if (value == "on;")
		loc.setAutoIndex(true);
	else if (value == "off;")
		loc.setAutoIndex(false);
	else
		throw syntaxError();
	// std::cout << "AUTOINDEX LOCATION OK" << std::endl;
}

void	confIndex(std::string value, Location &loc){
	value.pop_back();
	if (value.find_first_of("\t\n\v\f\r ;") != std::string::npos)
		throw syntaxError();
	// std::cout << "INDEX LOCATION OK" << std::endl;
	loc.setIndex(value);
}

void	confCGI(std::string value, Location &loc){
	if ((value == "yes;" || value == "y;"))
		loc.setCGI(true);
	else if (value == "no;" || value == "n;")
		loc.setCGI(false);
	else
		throw syntaxError();
	// std::cout << "CGI LOCATION OK" << std::endl;
}

void	confPath(std::string value, Location &loc){
	value.pop_back();
	if (value.find_first_of("\t\n\v\f\r ;") != std::string::npos)
		throw syntaxError();
	if (value[0] != '/')
		value = '/' + value;
	// std::cout << "PATH LOCATION OK" << std::endl;
	loc.setPath(value);
}

static std::string findKey(std::string str){
	if (str.find_first_of("\t\n\v\f\r ") != std::string::npos)
		return str.substr(0, str.find_first_of("\t\n\v\f\r "));
	else
		return str.substr(0, (str.length() - 1));
}

void	Location::autoConfig(Server &serv){
	std::list<std::string>::iterator it = _body.begin();
	std::string temp = serv.getRoot();
	_name = extractName(*it);
	_methods = serv.getMethods();
	_root = "";
	_autoIndex = serv.getAutoIndex();
	_index = serv.getIndex();
	_cgi = serv.getCGI();
	_path = "";
	it++;
	_body.pop_back();
	void (*ptr[7])(std::string, Location&) = {&confMethods, &confRedirect,
		&confRoot, &confAutoIndex, &confIndex, &confCGI, &confPath};
	std::string const keys[7] = {"allowedMethods", "return",
		"root", "autoIndex", "index", "cgiAllowed", "path"};
	while (it != _body.end()){
		std::string str = *it;
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		if (str.front() == '#'){
			it++;
			continue ;
		}
		if (str.back() != ';'){
			throw syntaxError();}
		std::string key = findKey(str);
		str.erase(0, key.length());
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		for (int i = 0; i < 7; i++){
			if (key == keys[i]){
				(*ptr[i])(str, *this);
				break;
			}
			if (i == 6){
				throw syntaxError();}
		}
		it++;
	}
	// temp == root of serv + _root here if find in location
	_root = temp + _root;										// ....path/upload/
	_path = _root + _path;										// .../path/upload/data/
	// std::cout << "LOCATION OK" << std::endl;
}

std::ostream & operator<< (std::ostream &out, const Location& src){

	out << "LOCATION\n";
	out << "\tname\t" << src.getName() << std::endl;
	out << "\troot\t" << src.getRoot() << std::endl;
	out << "\tmethods\t";
	for (int i = GET; i <= HEAD; i++){
		out << boolstring(src.getMethod(i)) << "\t";
	}
	out << std::endl;
	out << "\tcgi\t" << boolstring(src.getCGI()) << std::endl;
	out << "\tIndex\t" << src.getIndex() << std::endl;
	out << "\tauto Index\t" << boolstring(src.getAutoIndex()) << std::endl;
	for (s_redirect redir : src.getRedirect()){
		out << "\tValue\t" << redir.returnValue << "\tFROM " << redir.redirFrom 
			<< "\tTO " << redir.redirTo << std::endl;
	}
	out << "\tPATH\t" << src.getPath() << std::endl;
	return out;
}
