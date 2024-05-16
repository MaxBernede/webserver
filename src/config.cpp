#include "../inc/webserver.hpp"

void confPort(std::string value, Server &serv){
	s_port port;

	std::string num = value.substr(0, value.find_first_of("\t\n\v\f\r "));
	size_t pos = num.length();
	while (value.find_first_of("\t\n\v\f\r ", pos) == 0)
		pos++;
	port.type = value.substr(pos, value.length());
	port.type.pop_back();
	port.nmb = std::stol(num);
	serv.setPort(port);
}

void confName(std::string value, Server &serv){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	serv.setName(value);
}

void confRoot(std::string value, Server &serv){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	serv.setRoot(value);
}

void confMethods(std::string value, Server &serv){
	std::string const meth[8] = {"GET", "POST", "DELETE", "PUT", "PATCH", "CONNECT", "OPTIONS", "TRACE"};
	while (value != ";"){
		std::string tmp = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		for (int i = 0; i < 8; i++)
		{
			if (tmp == meth[i]){
				serv.setMethod(i, true);
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

void confCGI(std::string value, Server &serv){
	if (value == "yes;" || value == "y;")
		serv.setCGI(true);
	else if (value == "no;" || value == "n;")
		serv.setCGI(false);
	else
		throw syntaxError();
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
		case 'K':
			val *= (1 << 10);
	}
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
}

void confIndex(std::string value, Server &serv){
	while (value != ";"){
		std::string tmp = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
		value.erase(0, tmp.length());
		while (value.find_first_of("\t\n\v\f\r ") == 0)
			value.erase(0, 1);
		serv.setIndex(tmp);
	}
}

void confAutoIndex(std::string value, Server &serv){
	if (value == "on;")
		serv.setAutoIndex(true);
	else if (value == "off;")
		serv.setAutoIndex(false);
	else
		throw syntaxError();
}

void confRedirect(std::string value, Server &serv){
	
	std::string num = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
	if (num.length() != 3 || num.find_first_not_of("1234567890") != std::string::npos)
		throw syntaxError();
	value.erase(0, num.length());
	while (value.find_first_of("\t\n\v\f\r ") == 0)
		value.erase(0, 1);
	std::string from = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
	if (from.length() == 0)
		throw syntaxError();
	value.erase(0, from.length());
	while (value.find_first_of("\t\n\v\f\r ") == 0)
		value.erase(0, 1);
	std::string to = value.substr(0, value.find_first_of("\t\n\v\f\r ;"));
	if (to.length() == 0)
		throw syntaxError();
	value.erase(0, to.length());
	while (value.find_first_of("\t\n\v\f\r ") == 0)
		value.erase(0, 1);
	if (value != ";")
		throw syntaxError();
	s_redirect redir;
	redir.returnValue = std::stoi(num);
	redir.redirFrom = from;
	redir.redirTo = to;
	serv.setRedirect(redir);
}