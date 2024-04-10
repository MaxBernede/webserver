/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: kposthum <kposthum@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/03/21 15:45:36 by kposthum      #+#    #+#                 */
/*   Updated: 2024/04/10 15:10:48 by kposthum      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include"Server.hpp"
#include<string>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<exception>

class	invalidFile : public std::exception{
	public:
		const char	*what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW{
			return "Could not open file, resort to default config";
		}
};

class	syntaxError : public std::exception{
	public:
		const char	*what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW{
			return "Syntax error, resort to default config";
		}
};

bool servBlockStart(std::string buf){
	if (buf.back() != '{')
		return false;
	if (buf.find("server") != 0)
		return false;
	for (size_t i = 6; i < (buf.size() - 1); i++){
		if (buf[i] != 32 || (buf[i] < 9 && buf[i] > 13))
			return false;
	}
	return true;
}

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

std::string findKey(std::string str){
	if (str.find_first_of("\t\n\v\f\r ") != std::string::npos)
		return str.substr(0, str.find_first_of("\t\n\v\f\r "));
	else
		return str.substr(0, (str.length() - 1));
}

Server	pushBlock(std::list<std::string> block, char **env){
	Server serv(env);
	void (*ptr[9])(std::string, Server&) = {&confPort, &confName, &confRoot, &confMethods, &confCGI,
		&confMaxBody, &confErrorPage, &confIndex, &confAutoIndex};
	std::string const keys[9] = {"listen", "serverName", "root", "allowedMethods", "cgiAllowed",
		"clientMaxBodySize", "errorPage", "index", "autoIndex"};
	bool clear[10] = {false, false, false, false, false,
		false, false, false, false, false};
	for (std::string str : block){
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		if (str.front() == '#')
			continue;
		if (str.back() != ';')
			throw syntaxError();
		try{
			std::string key = findKey(str);
			str.erase(0, key.length());
			while (str.find_first_of("\t\n\v\f\r ") == 0)
				str.erase(0, 1);
			for (int i = 0; i < 10; i++){
				if (key == keys[i]){
					if (clear[i] == false){
						serv.clearData(i);
						clear[i] = true;
					}
					(*ptr[i])(str, serv);
					break;
				}
				if (i == 9)
					throw syntaxError();
			}
		}
		catch(std::exception const &e){
			std::cout << e.what() << std::endl;
			continue;
		}
	}
	return serv;
}

std::list<Server>	init_serv(std::ifstream &conf, char **env){
	std::list<Server> server;
	std::string buf;
	std::list<std::string> block;

	while (!conf.eof()){
		std::getline(conf, buf);
		if (servBlockStart(buf)){
			try{
				while (true){
					std::getline(conf, buf);
					if (buf.front() == '}')
						break;
					if (conf.eof())
						throw syntaxError();
					block.push_back(buf);
				}
				server.push_back(pushBlock(block, env));
				block.clear();
			}
			catch(std::exception &e){
				std::cout << e.what() << std::endl;
				Server def(env);
				server.push_front(def);
			}
		}
	}
	return server;
}

int main(int argc, char** argv, char** env){
	std::list<Server> server;
	(void)argc;
	try{
		std::ifstream conf(argv[1], std::ios::in);
		if (!conf.is_open())
			throw invalidFile();
		server = init_serv(conf, env);
	}
	catch(std::exception const &e){
		Server def(env);
		server.push_front(def);
	}
	for (Server i : server){
		std::cout << i << std::endl;
	}
	return 0;
}
