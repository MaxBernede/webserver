#include "Webserver.hpp"
#include "ServerRun.hpp"

#include <sys/types.h>
#include <dirent.h>

int main(int argc, char** argv) {

	// std::list<Server> server;

	// try{
	// 	if (argc != 2)
	// 		throw invalidFile();
	// 	std::ifstream conf(argv[1], std::ios::in);
	// 	if (!conf.is_open())
	// 		throw invalidFile();
	// 	server = init_serv(conf);
	// }
	// catch(std::exception const &e){
	// 	Server def;
	// 	server.push_front(def);
	// }
	// // for (Server s : server){
	// // 	std::cout << s << std::endl;
	// // }
	// // initialise ServerRun obj
	// ServerRun runningServer(server);
	// // Run server loop
	// runningServer.serverRunLoop();
	std::vector<std::string> v;
	DIR* dirp = opendir(".");
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
	for (std::string s : v)
		std::cout << "LIST:\t" << s << std::endl;
	return 0;
}
