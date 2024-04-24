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
		int _type;
<<<<<<< HEAD

	public:
		Socket(int port);
=======
		int _serverNum;

	public:
		Socket(int port, int serverNum);
>>>>>>> sock_class
		~Socket(void);

		Socket &operator=(Socket &other) noexcept;

<<<<<<< HEAD
		int	getFd( void );
=======
		int	getFd( void ) noexcept;
		int	getServerNum( void ) noexcept;
>>>>>>> sock_class
};
