#pragma once

#include <iostream>
#include <string>
#include <list>
#include <vector>

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

enum methods{
	GET,
	POST,
	DELETE,
	PUT,
	PATCH,
	CONNECT,
	OPTIONS,
	TRACE
};

typedef struct t_port{
	uint16_t	nmb;
	std::string	type;
}	s_port;

typedef struct t_ePage{
	uint16_t	err;
	std::string	url;
}	s_ePage;

class Server {

	private:
		uint32_t				_host;
		// std::string				_hoststring;
		std::list<s_port>		_ports;
		std::string 			_name;
		std::string 			_root;
		std::vector<bool>		_methods;
		bool					_cgi;
		uint64_t				_maxBody;
		std::list<s_ePage>		_errorPages;
		std::list<std::string>	_index;
		bool					_autoIndex;

	public:
		Server(char **env);
		~Server();
		Server(const Server &obj);
		Server &operator=(const Server &obj);

		uint32_t				getHost()	const;
		// std::string				getHoststring()	const;
		std::list<s_port>		getPorts()	const;
		std::string				getName()	const;
		std::string				getRoot()	const;
		bool					getMethod(int i)	const;
		std::vector<bool>		getMethods()	const;
		bool					getCGI()	const;
		uint64_t				getMaxBody()	const;
		std::list<s_ePage>		getErrorPages()	const;
		std::list <std::string>	getIndex()	const;
		bool					getAutoIndex()	const;

		void	setHost(uint32_t host);
		// void	setHostString(std::string host);
		void	setPort(s_port port);
		void	setName(std::string name);
		void	setRoot(std::string root);
		void	setMethod(int method, bool value);
		void	setCGI(bool CGI);
		void	setMaxBody(uint64_t body);
		void	setErrorPages(s_ePage ePage);
		void	setIndex(std::string index);
		void	setAutoIndex(bool autoIndex);

		void	clearHost();
		void	clearPort();
		void	clearName();
		void	clearRoot();
		void	clearMethods();
		void	clearCGI();
		void	clearMaxBody();
		void	clearEPage();
		void	clearIndex();
		void	clearAutoIndex();

		void	clearData(int index);
};

std::ostream & operator<< (std::ostream &out, const Server& src);

Server	pushBlock(std::list<std::string> block, char **env);
bool servBlockStart(std::string buf);
std::list<Server>	init_serv(std::ifstream &conf, char **env);