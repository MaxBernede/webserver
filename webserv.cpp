/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: kposthum <kposthum@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/03/21 15:45:36 by kposthum      #+#    #+#                 */
/*   Updated: 2024/03/27 13:58:25 by mbernede      ########   odam.nl         */
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

//TODO error handling
s_port confPort(std::string value){
	s_port port;

	std::string num = value.substr(0, value.find_first_of("\t\n\v\f\r "));
	size_t pos = num.length();
	while (value.find_first_of("\t\n\v\f\r ", pos) == 0)
		pos++;
	port.type = value.substr(pos, value.length());
	port.type.pop_back();
	port.nmb = std::stol(num);
	return port;
}

std::string confName(std::string value){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	//TODO check extention? //other error handling?
	return value;
}

std::string confRoot(std::string value){
	if (value.find_first_of("\t\n\v\f\r ") != std::string::npos)
		throw syntaxError();
	value.pop_back();
	//TODO-ish:check valid pathing? idk
	return value;
}

void confMethods(std::string value){
	//go over words one by one and check if they are valid methods
	//all given are set to true
	//if none are given, set all to false
	//if one inalid is found, throw exception
}

bool confCGI(std::string value){
	if (value == "yes;" || value == "y;")
		return true;
	else if (value == "no;" || value == "n;")
		return false;
	else
		throw syntaxError();
}

// TODO check for overflow?
uint32_t confMaxBody(std::string value){
	if (value.length() > 10)
		throw syntaxError();
	value.pop_back();
	if (value.find_first_of("KkMmGgBb") != value.length() - 1)
		throw syntaxError();
	if (value.find_first_not_of("1234567890KkMmGgBb") != std::string::npos)
		throw syntaxError();
	uint32_t val = (uint32_t)std::stol(value);
	if (value.back() == 'K' || value.back() == 'k')
		val = val * (1 < 10);
	else if (value.back() == 'M' || value.back() == 'm')
		val = val * ((1 < 10) < 10);
	else if (value.back() == 'G' || value.back() == 'g')
		val = val * (((1 < 10) < 10) < 10);
	if (val > BODY_MAX)
		throw syntaxError();
	return val;
}

// std::list<s_ePage> confErrorPage(std::string value){
	
// }

// std::list<std::string> confIndex(std::string value){

// }

bool confAutoIndex(std::string value){
	if (value == "yes;" || value == "y;")
		return true;
	else if (value == "no;" || value == "n;")
		return false;
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
	for (std::string str : block){
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		if (str.front() == '#')
			continue;
		if (str.back() != ';')
			throw syntaxError();
		try{
			std::string key = findKey(str);
			std::cout << "KEY <" << key << ">" << std::endl;
			str.erase(0, key.length());
			while (str.find_first_of("\t\n\v\f\r ") == 0)
				str.erase(0, 1);
			std::cout << "VALUE <" << str << ">" << std::endl;
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

	// std::cout << file;
	return server;
}

int main(int argc, char** argv, char** env){
	std::list<Server> server;

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
