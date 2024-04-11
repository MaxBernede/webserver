// #include "../inc/webserver.hpp"

// sig_atomic_t sigflag = 0;

// void sighandler(int s)
// {
// 	std::cerr << "Caught signal " << s << ".\n"; // this is undefined behaviour
// 	sigflag = 1; // something like that
// 	close(8080);
// 	// Exit the program
// 	exit(1);
// }