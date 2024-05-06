#pragma once

# include "Exception.hpp"

# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <netdb.h>

# include <netinet/in.h>
# include <cstdint>

class Socket
{
	private:
		int _fd;
		int _serverNum;

	public:
		Socket(int port);
		~Socket(void);

		Socket &operator=(Socket &other) noexcept;

		int	getFd( void );
};
