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
		int	_fd;
		std::string _addr;

	public:
		Socket();
		~Socket();

		Socket &operator=(Socket &other) noexcept;

		void bind(uint16_t port) const;
		void listen(int backlog) const;
		Socket accept() const;

		int	getFd( void );
		std::string &getAddr( void );
};

