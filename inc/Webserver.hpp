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
#include <filesystem>

// for containers
#include <vector>
#include <list>
#include <algorithm>

//for the signal
#include <iostream>
#include <csignal>

#define MIN_PORT 1
#define MAX_PORT UINT16_MAX

#include <Logger.hpp>
#include <Colors.hpp>
#include <HTTPStatus.hpp>
#include <Response.hpp>
#include<Utils.hpp>
#include<Server.hpp>
#include<Config.hpp>
#include<Location.hpp>
#include<Socket.hpp>
#include<ServerRun.hpp>
#include<Request.hpp>
#include<CGI.hpp>
#include <Error.hpp>
#include <FileHandling.hpp>