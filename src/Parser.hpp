#ifndef _PARSER_HPP_
#define _PARSER_HPP_
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "Server.hpp"
#include <map>

void setLocation(std::ifstream &file, Server &server, const std::string &path);
void parserConfig(std::ifstream &file, Server &server);

#endif 