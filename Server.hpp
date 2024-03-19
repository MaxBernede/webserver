#ifndef Template_HPP
#define Template_HPP

#include <iostream>
#include <string>

typedef struct port{
	uint16_t	nmb;
	std::string	type;
}	s_port;

typedef struct method{
	bool	GET;
	bool	POST;
	bool	DELETE;
	// etc
} s_method;

typedef struct ePage{
	uint16_t	err;
	std::string	url;
}	s_ePage;

class Server {

	private:
		s_port*		_ports;
		std::string _name;
		std::string _root;
		s_method	_methods;
		bool		_cgi;
		uint32_t	_max_body;
		s_ePage*	_error_pages;

	public:
		Server();
		Server(std::string);
		~Server();
		Server(const Server &obj);

		Server &operator=(const Server &obj);
};

#endif
