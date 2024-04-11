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
		int fd;
		struct addrinfo *serv_addr;

	public:
		Socket(uint16_t port);
		~Socket(void);

		Socket &operator=(Socket &other) noexcept;

		void bind( void ) const;
		void listen(int backlog) const;
		int accept() const;

		int	getFd( void );
		std::string &getAddr( void );
};

