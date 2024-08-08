#pragma once

#include "Server.hpp"

void confPort(std::string value, Server& serv);
void confName(std::string value, Server& serv);
void confRoot(std::string value, Server& serv);
void confMethods(std::string value, Server& serv);
void confCGI(std::string value, Server& serv);
void confMaxBody(std::string value, Server& serv);
void confErrorPage(std::string value, Server& serv);
void confIndex(std::string value, Server& serv);
void confAutoIndex(std::string value, Server& serv);
void confRedirect(std::string value, Server& serv);
void confPath(std::string value, Server& serv);

void addLocation(std::list<std::string>& body, Server& serv);