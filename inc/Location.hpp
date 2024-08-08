#pragma once

#include<Utils.hpp>
#include<ConfigClass.hpp>
#include"Server.hpp"

#include<string>
#include<list>
#include<vector>
#include<iostream>

class Server;

class Location : public Config {
private:
	// name is what the request provides
	std::string				_name;
	// what the root directory of this location is
	// should be located within the parent config block
	std::string 			_root;
	// config body, used only for parsing
	std::list<std::string>	_body;
public:
	Location(std::list<std::string>& body);
	~Location();
	Location(const Location& obj);
	Location& operator=(const Location& obj);

	std::string				getName() const;
	std::string				getRoot() const;

	void	setName(std::string name);
	void	setMethod(int method, bool value);
	void	setRoot(std::string root);

	void	autoConfig(Server& serv);
};

std::ostream& operator<< (std::ostream& out, const Location& src);
