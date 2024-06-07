#pragma once

#include<utils.hpp>
#include<Server.hpp>

#include<string>
#include<list>
#include<vector>
#include<iostream>

class Server;

class Location{
	private:
		std::string				_name;
		std::vector<bool>		_methods;
		std::list<s_redirect>	_redirect;
		std::string 			_root;
		bool					_autoIndex;
		std::string				_index;
		bool					_cgi;
		std::string				_path;
		std::list<std::string>	_body;
	public:
		Location(std::list<std::string> &body);
		~Location();
		Location(const Location &obj);
		Location &operator=(const Location &obj);

		std::string				getName() const;
		std::vector<bool>		getMethods() const;
		bool					getMethod(int i) const;
		std::list<s_redirect>	getRedirect() const;
		std::string				getRoot() const;
		bool					getAutoIndex() const;
		std::string				getIndex() const;
		bool					getCGI() const;
		std::string				getPath() const;

		void	setName(std::string name);
		void	setMethod(int method, bool value);
		void	setRedirect(s_redirect redir);
		void	setRoot(std::string root);
		void	setAutoIndex(bool autoIndex);
		void	setIndex(std::string index);
		void	setCGI(bool CGI);

		void	autoConfig(Server &serv);
};

std::ostream & operator<< (std::ostream &out, const Location& src);
