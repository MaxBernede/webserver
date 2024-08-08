#pragma once

#include <ConfigClass.hpp>
#include <Location.hpp>
#include <iostream>
#include <string>
#include <list>
#include <vector>

class Location;

// main confic block, the program will loop through these to search for the right config
// based on the recieved request
class Server : public Config {

private:
	// what host::port this config should be listening to
	std::list<s_domain>		_ports;
	// name of the server, I don't think this does anything unless actually connected to the internet
	std::string 			_name;
	// root directory in which its files are stored
	std::string 			_root;
	// the maximum number of allowed bytes per request
	uint64_t				_maxBody;
	// definition of location of specific error pages
	std::list<s_ePage>		_errorPages;
	// a list of locations within the config (see Location.hpp for more details)
	std::list<Location>		_location;

public:
	Server();
	~Server();
	Server(const Server& obj);
	Server& operator=(const Server& obj);

	std::list<s_domain>		getPorts()	const;
	std::string				getName()	const;
	std::string				getRoot()	const;
	uint64_t				getMaxBody()	const;
	std::list<s_ePage>		getErrorPages()	const;
	std::list<Location>		getLocation() const;
	Location				getLocationByName(const std::string& name) const;

	void	setPort(s_domain port);
	void	setName(std::string name);
	void	setRoot(std::string root);
	void	setMaxBody(uint64_t body);
	void	setErrorPages(s_ePage ePage);
	void	setLocation(Location& locaiton);

	void	configLocation();
	void	confErr();

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
	void	clearPath();

	void	clearData(int index);
};

std::ostream& operator<< (std::ostream& out, const Server& src);

Server	pushBlock(std::list<std::string> block);
bool	servBlockStart(std::string buf);
std::list<Server>	init_serv(std::ifstream& conf);
