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

Server	pushBlock(std::list<std::string> block, char **env){
	Server serv(env);
	for (std::string str : block){
		if (str.back() != ';')
			throw syntaxError();
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
				while (buf.back() != '}'){
					if (conf.eof())
						throw syntaxError();
					std::getline(conf, buf);
					block.push_back(buf);
				}
				server.push_back(pushBlock(block, env));
				block.clear();
			}
			catch(std::exception &e){
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
