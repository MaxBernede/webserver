#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>

// #define MIN_PORT 1
// #define MAX_PORT UINT16_MAX

#define MIN_PORT 8000
#define MAX_PORT 8100


//What doesnt need anything
#include "Colors.hpp"
#include "HTTPStatus.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Error.hpp"

#include "Response.hpp"

#include "ConfigClass.hpp"
#include "OurClasses.hpp"

#include "HTTPError.hpp"

#include "Location.hpp"

#include "Server.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "ServerRun.hpp"
#include "Request.hpp"
#include "CGI.hpp"
#include "FileHandling.hpp"
#include "HTTPObject.hpp"
