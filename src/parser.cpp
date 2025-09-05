#include <string>
#include <fstream>
#include "Server.hpp"
#include <map>

void parser(std::ifstream &file, Server &server)
{
    std::string line;
    size_t strSize = 0;
    std::map<std::string, void(Server::*)(const std::string&)> setters;
    setters["host"] = &Server::setHost;
    setters["port"] = &Server::setPort;
    setters["server_name"] = &Server::setServerName;
    setters["root"] = &Server::setRoot;
    setters["index"] = &Server::setIndex;
    setters["error_page"] = &Server::setErrorPages;
    setters["client_max_body_size"] = &Server::setClientMaxBodySize;


    std::map<std::string, void(Server::*)(const std::string&)>::iterator curr;

    while (std::getline(file, line))
    {
        for (curr = setters.begin(); curr != setters.end(); curr++)
        {
            if (line.find(curr->first) != std::string::npos)
            {
                strSize = line.find(curr->first);
                std::string value = line.substr(strSize + curr->first.size() + 1);
                value.erase(value.size() - 1);
                (server.*(curr->second))(value);
            }  
        }
    }
}

// int main()
// {
//     Server server;

//     std::ifstream file("../config.conf");
//     parser(file, server);
// }
