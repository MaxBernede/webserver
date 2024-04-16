#include "webserver.hpp"

EpollQueue::EpollQueue( void )
{
	// create an epoll instance with epoll_create1();
}

EpollQueue::~EpollQueue( void )
{
}

void EpollQueue::add(int fd)
{
	// epoll_ctl(ADD)
}

void EpollQueue::remove(int fd)
{
	// epol_ctl(DEL)
}

void EpollQueue::modify(int fd)
{
	//epoll_ctl(MOD)
}

void EpollQueue::wait()
{
}
