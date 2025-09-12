#ifndef _PARSER_HPP_
#define _PARSER_HPP_
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "Server.hpp"
#include <map>

void setLocation(std::ifstream &file, Server &server);
void parserConfig(std::ifstream &file, Server &server);

#endif 