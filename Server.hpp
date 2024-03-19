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
	// etc?
} s_method;

typedef struct ePage{
	uint16_t	err;
	std::string	url;
}	s_ePage;

class Server {

	private:
		s_port*			_ports;
		std::string 	_name;
		std::string 	_root;
		s_method		_methods;
		bool			_cgi;
		uint32_t		_maxBody;
		s_ePage*		_errorPages;
		std::string*	_index;
		bool			_autoIndex;

	public:
		Server(char **env);
		Server(char *conf, char **env);
		~Server();
		Server(const Server &obj);

		Server &operator=(const Server &obj);
};

#endif
