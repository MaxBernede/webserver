#pragma once

//DATAS TYPES
#include <unordered_map>
#include <string>

#include <filesystem>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>

// for containers
#include <vector>
#include <list>
#include <algorithm>

//for the signal
#include <iostream>
#include <csignal>

#define MIN_PORT 8000
#define MAX_PORT 8100

#include "utils.hpp"
#include "signal.hpp"
#include "server.hpp"
#include "config.hpp"
#include "Socket.hpp"
#include "ServerRun.hpp"
#include "request.hpp"
#include "response.hpp"
#include "print_colors.hpp"
