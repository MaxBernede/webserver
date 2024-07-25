#pragma once

#include "Exception.hpp"
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
		Socket(int port);
		~Socket(void);

		Socket &operator=(Socket &other) noexcept;

		int	getFd( void );
};
