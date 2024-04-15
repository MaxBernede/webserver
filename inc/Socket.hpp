#pragma once

# include "Exception.hpp"

# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <netdb.h>

# include <netinet/in.h>
# include <cstdint>

// move semantics for allocating values from another class?
class Socket
{
	private:
		int _fd;
		std::string hostName

	public:
		Socket(s_port port);
		~Socket(void);

		Socket &operator=(Socket &other) noexcept;

		int accept() const;

		int	getFd( void );
};
