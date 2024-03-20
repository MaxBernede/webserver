#ifndef Template_HPP
#define Template_HPP

#include <iostream>
#include <string>
#include<list>

typedef struct t_port{
	uint16_t	nmb;
	std::string	type;
}	s_port;

typedef struct t_method{
	bool	GET;
	bool	POST;
	bool	DELETE;
	// etc?
} s_method;

typedef struct t_ePage{
	uint16_t	err;
	std::string	url;
}	s_ePage;

class Server {

	private:
		std::list<s_port>		_ports;
		std::string 			_name;
		std::string 			_root;
		s_method				_methods;
		bool					_cgi;
		uint32_t				_maxBody;
		std::list<s_ePage>		_errorPages;
		std::list<std::string>	_index;
		bool					_autoIndex;

	public:
		Server(char **env);
		Server(char *conf, char **env);
		~Server();
		Server(const Server &obj);
		Server &operator=(const Server &obj);

		std::list<s_port>		getPorts()	const;
		std::string				getName()	const;
		std::string				getRoot()	const;
		s_method				getMethods()	const;
		bool					getCGI()	const;
		uint32_t				getMaxBody()	const;
		std::list<s_ePage>		getErrorPages()	const;
		std::list <std::string>	getIndex()	const;
		bool					getAutoIndex()	const;
};

std::ostream & operator<< (std::ostream &out, const Server& src);

#endif
