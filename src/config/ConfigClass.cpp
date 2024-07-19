/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigClass.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: kposthum <kposthum@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/02 13:12:16 by kposthum      #+#    #+#                 */
/*   Updated: 2024/07/19 15:10:43 by mbernede      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigClass.hpp"
// #include "webserver.hpp"

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

static std::string defaultIndex(){
	return "index.html";
}

static std::list<s_redirect> defaultRedirect(){
	std::list<s_redirect> redirect;
	redirect.clear();
	return redirect;
}

Config::Config():
	_methods(defaultMethods()),
	_redirect(defaultRedirect()),
	_autoIndex(true),
	_index(defaultIndex()),
	_cgi(true) {
}


Config::~Config(){}

Config::Config(const Config &obj){
	*this = obj;
}

Config &Config::operator=(const Config &obj){
	this->_methods = obj.getMethods();
	this->_redirect = obj.getRedirect();
	this->_autoIndex = obj.getAutoIndex();
	this->_index = obj.getIndex();
	this->_cgi = obj.getCGI();
	this->_path = obj.getPath();
	return *this;
}

std::vector<bool>	Config::getMethods() const{
	return _methods;
}

bool	Config::getMethod(int i) const{
	return _methods[i];
}

std::list<s_redirect>	Config::getRedirect() const{
	return _redirect;
}

bool	Config::getAutoIndex() const{
	return _autoIndex;
}

std::string	Config::getIndex() const{
	return _index;
}

bool	Config::getCGI() const{
	return _cgi;
}

std::string	Config::getPath() const{
	return _path;
}

void	Config::setMethod(int method, bool value){
	_methods[method] = value;
}

void	Config::setRedirect(s_redirect redir){
	_redirect.push_back(redir);
}

void	Config::setRedirect(std::list<s_redirect> redir){
	// _redirect.push_back(redir);
	_redirect = redir;
}

void	Config::setAutoIndex(bool autoIndex){
	_autoIndex = autoIndex;
}

void	Config::setIndex(std::string index){
	_index = index;
}

void	Config::setCGI(bool CGI){
	_cgi = CGI;
}

void	Config::setPath(std::string path){
	_path = path;
}
