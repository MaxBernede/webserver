#pragma once

#include "server.hpp"
#include "webserver.hpp"

class EpollQueue {
	private:
		int _epoll_fd;

	public:
		EpollQueue( void );
		~EpollQueue( void );

		void add(int fd);
		void remove(int) fd;
		void wait();
};
