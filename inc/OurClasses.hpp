#pragma once
#include <exception>
#include <string>

enum ErrorCode {
    OK = 200,
	NO_CONTENT = 204,
	BAD_REQUEST = 400,
    FORBIDDEN = 403,
    PAGE_NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
    CONFLICT = 409,
	URI_TOO_LONG = 414,
	INTERNAL_SRV_ERR = 500,
	METHOD_NOT_IMPLEMENTED = 501,
	HTTP_NOT_SUPPORT = 505
};

class	invalidFile : public std::exception{
	public:
		const char	*what() const noexcept override {
			return "Could not open file, resort to default config";
		}
};

class	syntaxError : public std::exception{
	public:
		const char	*what() const noexcept override {
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
	TRACE,
	HEAD,
	ENUM_SIZE
};

typedef struct t_domain{
	uint16_t	port;
	uint32_t	host;
}	s_domain;

typedef struct t_ePage{
	uint16_t	err;
	std::string	url; 
}	s_ePage;

typedef struct t_redirect{
	uint16_t	returnValue;
	std::string	redirFrom;
	std::string	redirTo;
}	s_redirect;

// class Config;
// class Location;
// class Server;