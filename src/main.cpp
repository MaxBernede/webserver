#include "Webserver.hpp"
#include "ServerRun.hpp"

#include <sys/types.h>
#include <dirent.h>

int main(int argc, char** argv) {

	std::list<Server> server;

	try {
		if (argc != 2)
			throw invalidFile();
		std::ifstream conf(argv[1], std::ios::in);
		if (!conf.is_open())
			throw invalidFile();
		server = init_serv(conf);
	}
	catch (std::exception const& e) {
		Server def;
		server.push_front(def);
	}
	ServerRun runningServer(server);
	runningServer.serverRunLoop();
	return 0;
}
