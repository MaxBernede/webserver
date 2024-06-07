#pragma once

// #include<utils.hpp>
#include<Location.hpp>
#include<iostream>
#include<string>
#include<list>
#include<vector>

class Location;

class Server {

	private:
		std::list<s_port>		_ports;
		std::string 			_name;
		std::string 			_root;
		std::vector<bool>		_methods;
		bool					_cgi;
		uint64_t				_maxBody;
		std::list<s_ePage>		_errorPages;
		std::string				_index;
		bool					_autoIndex;
		std::list<s_redirect>	_redirect;
		std::list<Location>		_location;

	public:
		Server();
		Server(char **env);
		~Server();
		Server(const Server &obj);
		Server &operator=(const Server &obj);

		std::list<s_port>		getPorts()	const;
		std::string				getName()	const;
		std::string				getRoot()	const;
		bool					getMethod(int i)	const;
		std::vector<bool>		getMethods()	const;
		bool					getCGI()	const;
		uint64_t				getMaxBody()	const;
		std::list<s_ePage>		getErrorPages()	const;
		std::string				getIndex()	const;
		bool					getAutoIndex()	const;
		std::list<s_redirect>	getRedirect() const;
		std::list<Location>		getLocation() const;

		void	setPort(s_port port);
		void	setName(std::string name);
		void	setRoot(std::string root);
		void	setMethod(int method, bool value);
		void	setCGI(bool CGI);
		void	setMaxBody(uint64_t body);
		void	setErrorPages(s_ePage ePage);
		void	setIndex(std::string index);
		void	setAutoIndex(bool autoIndex);
		void	setRedirect(s_redirect redir);
		void	setLocation(Location locaiton);

		void	configLocation();

		void	clearPort();
		void	clearName();
		void	clearRoot();
		void	clearMethods();
		void	clearCGI();
		void	clearMaxBody();
		void	clearEPage();
		void	clearIndex();
		void	clearAutoIndex();
		void	clearRedirect();

		void	clearData(int index);
};

std::ostream & operator<< (std::ostream &out, const Server& src);

Server	pushBlock(std::list<std::string> block, char **env);
bool servBlockStart(std::string buf);
std::list<Server>	init_serv(std::ifstream &conf, char **env);