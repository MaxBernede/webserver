#pragma once

// #include<utils.hpp>
#include<Location.hpp>
#include<iostream>
#include<string>
#include<list>
#include<vector>

class Location;

//main confic block, the program will loop through these to search for the right config
// based on the recieved request
class Server {

	private:
		// what host::port this config should be listening to
		std::list<s_port>		_ports;
		// name of the server, I don't think this does anything unless actually connected to the internet
		std::string 			_name;
		// root directory in which its files are stored
		std::string 			_root;
		// which methods are allowed
		std::vector<bool>		_methods;
		// whether common gateway interface is allowed
		bool					_cgi;
		// the maximum number of allowed bytes per request
		uint64_t				_maxBody;
		// definition of location of specific error pages
		std::list<s_ePage>		_errorPages;
		// "home page", or default page to respond with when no specific file is requested
		std::string				_index;
		// (dis)allow directory listing
		bool					_autoIndex;
		// http redirect definitions
		std::list<s_redirect>	_redirect;
		// a list of locations within the config (see Location.hpp for more details)
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
		void	setLocation(Location &locaiton);

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