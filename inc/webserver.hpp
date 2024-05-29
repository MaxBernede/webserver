#pragma once

#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <unordered_map>

// for containers
#include <vector>
#include <list>
#include <algorithm>

//for the signal
#include <iostream>
#include <csignal>

#define MIN_PORT 8000
#define MAX_PORT 8100

#include "response.hpp"
#include "utils.hpp"
#include "signal.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include <Location.hpp>
#include "Socket.hpp"
#include "ServerRun.hpp"
#include "request.hpp"
#include "print_colors.hpp"
#include "CGI.hpp"