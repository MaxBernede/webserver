#pragma once

#include "Exception.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>

class Socket
{
	private:
		int _fd;

	public:
		Socket(std::string name);
		Socket(s_domain domain);
		~Socket(void);

		void	fillStruct(struct addrinfo &hints);
		Socket	&operator=(Socket &other) noexcept;

		int	getFd( void );
};
