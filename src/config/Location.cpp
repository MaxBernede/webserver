#include<Location.hpp>

Location::Location(std::list<std::string> &body){
	// _body = body;
	for (std::string s : body)
		_body.push_back(s);
	// std::cout << "TEST\n" << std::endl;
	// for (std::string i : _body){
	// 	std::cout << i << std::endl;
	// }
}

Location::~Location(){}

Location::Location(const Location &obj) : _body(obj._body){
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
	return *this;
}

std::string	Location::getName() const{
	return _name;
}

std::vector<bool>	Location::getMethods() const{
	return _methods;
}

bool	Location::getMethod(int i) const{
	return _methods[i];
}

std::list<s_redirect>	Location::getRedirect() const{
	return _redirect;
}

std::string	Location::getRoot() const{
	return _root;
}

bool	Location::getAutoIndex() const{
	return _autoIndex;
}

std::string	Location::getIndex() const{
	return _index;
}

bool	Location::getCGI() const{
	return _cgi;
}

std::string	Location::getPath() const{
	return _path;
}

void	Location::setName(std::string name){
	_name = name;
}

void	Location::setMethod(int method, bool value){
	_methods[method] = value;
}

void	Location::setRedirect(s_redirect redir){
	_redirect.push_back(redir);
}

void	Location::setRoot(std::string root){
	_root = root;
}

void	Location::setAutoIndex(bool autoIndex){
	_autoIndex = autoIndex;
}

void	Location::setIndex(std::string index){
	_index = index;
}

void	Location::setCGI(bool CGI){
	_cgi = CGI;
}

static std::string	extractName(std::string const &src){
	std::string name = src;
	name.erase(0, 9);
	name.erase(name.length() - 2, 2);
	// std::cout << "<" << name << ">" << std::endl;
	if (name.length() < 2)
		throw syntaxError();
	return name;
}

void	confMethods(std::string value, Server &serv, Location &loc){
	std::string const meth[8] = {"GET", "POST", "DELETE", "PUT",
		"PATCH", "CONNECT", "OPTIONS", "TRACE"};
	for (int i = 0; i < 8; i++){
		loc.setMethod(i, false);
	}
	while (value != ";"){
		std::string tmp = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		for (int i = 0; i < 8; i++)
		{
			if (tmp == meth[i] && serv.getMethod(i) == true){
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
}

void	confRedirect(std::string value, Server &serv, Location &loc){

}

void	confRoot(std::string value, Server &serv, Location &loc){

}

void	confAutoIndex(std::string value, Server &serv, Location &loc){

}

void	confIndex(std::string value, Server &serv, Location &loc){

}

void	confCGI(std::string value, Server &serv, Location &loc){

}

void	confPath(std::string value, Server &serv, Location &loc){

}

static std::string findKey(std::string str){
	if (str.find_first_of("\t\n\v\f\r ") != std::string::npos)
		return str.substr(0, str.find_first_of("\t\n\v\f\r "));
	else
		return str.substr(0, (str.length() - 1));
}

void	Location::autoConfig(Server &serv){
	// std::cout << "why" << std::endl;
	std::list<std::string>::iterator it = _body.begin();
	_name = extractName(*it);
	_methods = serv.getMethods();
	_root = serv.getRoot();
	_autoIndex = serv.getAutoIndex();
	_index = serv.getIndex();
	_cgi = serv.getCGI();
	_path = _root;
	it++;
	_body.pop_back();
	void (*ptr[7])(std::string, Server&, Location&) = {&confMethods, &confRedirect,
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
			// std::cout << "here1" << std::endl;
			throw syntaxError();}
		std::string key = findKey(str);
		// std::cout << "key\t" << key << std::endl;
		str.erase(0, key.length());
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		for (int i = 0; i < 7; i++){
			if (key == keys[i]){
				(*ptr[i])(str, serv, *this);
				break;
			}
			if (i == 6){
			// std::cout << "here2" << std::endl;
				throw syntaxError();}
		}
		it++;
	}
	// std::cout << *this << std::endl;
}

std::ostream & operator<< (std::ostream &out, const Location& src){

	out << "LOCATION\t";
	out << "name\t" << src.getName() << std::endl;
	out << "root\t" << src.getRoot() << std::endl;
	out << "methods\t";
	for (int i = GET; i <= TRACE; i++){
		out << boolstring(src.getMethod(i)) << "\t";
	}
	out << std::endl;
	out << "cgi\t" << boolstring(src.getCGI()) << std::endl;
	out << "Index\t" << src.getIndex() << std::endl;
	out << "auto Index\t" << boolstring(src.getAutoIndex()) << std::endl;
	for (s_redirect redir : src.getRedirect()){
		out << "Value\t" << redir.returnValue << "\tFROM " << redir.redirFrom 
			<< "\tTO " << redir.redirTo << std::endl;
	}
	out << "PATH\t" << src.getPath() << std::endl;
	return out;
}