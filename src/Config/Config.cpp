#include "Webserver.hpp"
#include "Server.hpp"
#include "Utils.hpp"

void confPort(std::string value, Server &serv){
	s_domain port;
	std::string ip = value.substr(0, value.find_first_of(":"));
	std::string num = value.substr((value.find_first_of(":") + 1), (value.length() - ip.length() - 1));
	num.pop_back();
	if (ip == "localhost")
		port.host = "127.0.0.1";
	else{
		if (ip.find_first_not_of("1234567890.") != std::string::npos)
			throw syntaxError();
		int j = 0;
		for (size_t i = 0; ip[i]; i++){
			if (ip[i] == '.')
				j++;
			if (j > 3)
				throw syntaxError();
		}
		if (j != 3)
			throw syntaxError();
		port.host = ip;
	}
	if (num.find_first_not_of("1234567890") != std::string::npos || num.length() < 1
		|| num.length() > 5 || std::stoi(num) > UINT16_MAX)
		throw syntaxError();
	port.port = std::stol(num);
	// std::cout << "PORT OK" << std::endl;
	serv.setPort(port);
}

void confName(std::string value, Server &serv){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	// std::cout << "NAME OK" << std::endl;
	serv.setName(value);
}

void confRoot(std::string value, Server &serv){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	// std::cout << "ROOT OK" << std::endl;
	serv.setRoot(value);
}

void confMethods(std::string value, Server &serv){
	std::string const meth[9] = {"GET", "POST", "DELETE", "PUT",
		"PATCH", "CONNECT", "OPTIONS", "TRACE", "HEAD"};
	while (value != ";"){
		std::string tmp = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		for (int i = GET; i <= HEAD; i++)
		{
			if (tmp == meth[i]){
				serv.setMethod(i, true);
				break;
			}
			if (i == HEAD)
				throw syntaxError();
		}
		value.erase(0, tmp.length());
		while (value.find_first_of("\t\n\v\f\r ") == 0)
			value.erase(0, 1);
	}
	// std::cout << "METHODS OK" << std::endl;
}

void confCGI(std::string value, Server &serv){
	if (value == "yes;" || value == "y;")
		serv.setCGI(true);
	else if (value == "no;" || value == "n;")
		serv.setCGI(false);
	else
		throw syntaxError();
	// std::cout << "CGI OK" << std::endl;
}

void confMaxBody(std::string value, Server &serv){
	if (value.length() > 10)
		throw syntaxError();
	value.pop_back();
	if (value.find_first_of("BKM") != value.length() - 1 && value.find_first_of("BKM") != std::string::npos)
		throw syntaxError();
	if (value.find_first_not_of("1234567890BKM") != std::string::npos)
		throw syntaxError();
	uint64_t val = (uint64_t)std::stol(value);
	switch (value.back()){
		case 'M':
			val *= (1 << 10);
			[[fallthrough]];
		case 'K':
			val *= (1 << 10);
		break ;
	}
	// std::cout << "BODY OK" << std::endl;
	serv.setMaxBody(val);
}

void confErrorPage(std::string value, Server &serv){
	while (value != ";"){
		std::string num = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		if (num.length() != 3 || num.find_first_not_of("1234567890") != std::string::npos)
			throw syntaxError();
		value.erase(0, num.length());
		while (value.find_first_of("\t\n\v\f\r ") == 0)
			value.erase(0, 1);
		std::string tmp = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		if (tmp.length() == 0)
			throw syntaxError();
		value.erase(0, tmp.length());
		while (value.find_first_of("\t\n\v\f\r ") == 0)
			value.erase(0, 1);
		s_ePage ePage;
		ePage.err = std::stoi(num);
		ePage.url = tmp;
		serv.setErrorPages(ePage);
	}
	// std::cout << "ERROR OK" << std::endl;

}

void confIndex(std::string value, Server &serv){
	value.pop_back();
	if (value.find_first_of("\t\n\v\f\r ;") != std::string::npos)
		throw syntaxError();
	serv.setIndex(value);
	// std::cout << "INDEX OK" << std::endl;
}

void confAutoIndex(std::string value, Server &serv){
	if (value == "on;")
		serv.setAutoIndex(true);
	else if (value == "off;")
		serv.setAutoIndex(false);
	else
		throw syntaxError();
	// std::cout << "AUTOINDEX OK" << std::endl;
}

void confRedirect(std::string value, Server &serv){
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
	// std::cout << "REDIR OK" << std::endl;
	serv.setRedirect(redir);
}

void	confPath(std::string value, Server &serv){
	value.pop_back();
	if (value.find_first_of("\t\n\v\f\r ;") != std::string::npos)
		throw syntaxError();
	// if (*value.end() != '/')
	// 	value = value + '/';
	// std::cout << "PATH OK" << std::endl;
	serv.setPath(value);
}

void addLocation(std::list<std::string> &body, Server &serv){
	Location loc(body);
	serv.setLocation(loc);
}