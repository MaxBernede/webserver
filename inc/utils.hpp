#pragma once

#include "webserver.hpp"

std::string	firstWord(const std::string& str);
bool		endsWith(const std::string& str, const std::string& suffix);
bool 		exists (const std::string& name);
void		create_file(std::string const &content, std::string const &location);
std::string getExtension(std::string fileName);
uint32_t	configIP(std::string ip);
std::string boolstring(const bool& src);

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
	uint32_t	host;
	uint16_t	port;
}	s_port;

typedef struct t_ePage{
	uint16_t	err;
	std::string	url;
}	s_ePage;

typedef struct t_redirect{
	uint16_t	returnValue;
	std::string	redirFrom;
	std::string	redirTo;
}	s_redirect;
