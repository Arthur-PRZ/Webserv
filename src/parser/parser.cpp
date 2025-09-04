#include <iostream>
#include <string>
#include <fstream>
#include "Server.hpp"
#include <map>

void parser(std::ifstream &file, Server &server)
{
    std::string line;
    size_t strSize = 0;
    std::map<std::string, void(Server::*)(const std::string&)> setters =
    {
        {"host",     &Server::setHost},
        {"port",     &Server::setPort},
        {"    ",     &Server::set},
        {"    ",     &Server::set},
        {"    ",     &Server::set},
        {"    ",     &Server::set},
        {"    ",     &Server::set},
        {"    ",     &Server::set},
        {"    ",     &Server::set},
    };

    std::map<std::string, void(Server::*)(const std::string&)>::iterator curr;

    while (std::getline(file, line))
    {
        for (curr = setters.begin(); curr != setters.end(); curr++)
        {
            if (line.find(curr->first) != std::string::npos)
            {
                Server server;

                strSize = line.find(curr->first);
                std::string value = line.substr(strSize + curr->first.size() + 1);
                value.pop_back();
                std::cout << value << std::endl;
                (server.*(curr->second))(value);
            }  
        }
    }
}