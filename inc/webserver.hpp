#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>

//for the signal
#include <iostream>
#include <csignal>

#define MIN_PORT 8000
#define MAX_PORT 8100

#include "utils.hpp"
#include "response.hpp"
#include "signal.hpp"
#include "server.hpp"
#include "config.hpp"
#include "Socket.hpp"