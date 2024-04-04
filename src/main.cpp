#include "Template.hpp"
#include <stack>

# define DEFAULT_CFG "template.conf"

int main(int argc, char **argv)
{
	std::string cfg_file  = DEFAULT_CFG;

	if (argc == 2)
		cfg_file = argv[1];
	
	std::cout << "Template for Webserv" << std::endl;
	return 0;
}