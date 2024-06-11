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
		// name is what the request provides
		std::string				_name;
		// which methods are allowed in tihs location
		// will be set to false if the not allowed in parent config block
		std::vector<bool>		_methods;
		// http redirection declaration
		std::list<s_redirect>	_redirect;
		// what the root directory of this location is
		// should be located within the parent config block
		std::string 			_root;
		// allow directory listing
		// will be false if not allowed in parent config block
		bool					_autoIndex;
		// home page of this location (usually a .html file)
		std::string				_index;
		// allow cgi requests
		// will be false if not allowed in parent config block
		bool					_cgi;
		// in case of POST or DELETE request:
		// directory in which to POST of from which to DELETE
		std::string				_path;
		// config body, used only for parsing
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
		void	setPath(std::string value);

		void	autoConfig(Server &serv);
};

std::ostream & operator<< (std::ostream &out, const Location& src);
