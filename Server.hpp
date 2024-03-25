#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include<list>
#include<vector>

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
		std::list<s_port>		_ports;
		std::string 			_name;
		std::string 			_root;
		std::vector<bool>		_methods;
		bool					_cgi;
		uint32_t				_maxBody;
		std::list<s_ePage>		_errorPages;
		std::list<std::string>	_index;
		bool					_autoIndex;

	public:
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
		uint32_t				getMaxBody()	const;
		std::list<s_ePage>		getErrorPages()	const;
		std::list <std::string>	getIndex()	const;
		bool					getAutoIndex()	const;

		void clearPort();
		void clearEPage();
		void clearIndex();
		void clearMethods();
		void setPort(s_port port);
		void setName(std::string name);
		void setRoot(std::string root);
		void setMethod(int method, bool value);
		void setCGI(bool CGI);
		void setMaxBody(uint32_t body);
		void setErrorPages(s_ePage ePage);
		void setIndex(std::string index);
		void setAutoIndex(bool autoIndex);
};

std::ostream & operator<< (std::ostream &out, const Server& src);

#endif
