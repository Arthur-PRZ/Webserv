#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "Server.hpp"
#include <map>

void setLocation(std::ifstream &file, Server &server)
{
    std::string line;
    int strSize = 0;
    Location newLocation;
    std::map<std::string, void(Location::*)(const std::string&)> setters;
    setters["methods"] = &Location::setMethods;
    setters["root"] = &Location::setRoot;
    setters["cgi_extension"] = &Location::setCGIExtensions;
    setters["upload_path"] = &Location::setUploadsPath;
    setters["redirect"] = &Location::setRedirect;

    std::map<std::string, void(Location::*)(const std::string&)>::iterator curr;

    std::map<std::string, void(Location::*)(const bool)> autoindex;
    autoindex["autoindex"] = &Location::setAutoIndex;


    while (std::getline(file, line))
    {
        if (line.find("}") != std::string::npos)
            break ;
        for (curr = setters.begin(); curr != setters.end(); curr++)
        {
            if (line.find(curr->first) != std::string::npos)
            {
                strSize = line.find(curr->first);
                std::string value = line.substr(strSize + curr->first.size() + 1);
                value.pop_back();
                (newLocation.*(curr->second))(value);
            }

        }
        if (line.find("autoindex") != std::string::npos)
        {
            if (line.find("off"))
                newLocation.setAutoIndex(false);
            else
                newLocation.setAutoIndex(true);
        }
    server.addLocation(newLocation);
    }
}

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

    std::map<std::string, void(Server::*)(const std::vector<Location>&)> location;
    location["host"] = &Server::setLocations;

    std::map<std::string, void(Server::*)(const std::string&)>::iterator curr;

    while (std::getline(file, line))
    {
        for (curr = setters.begin(); curr != setters.end(); curr++)
        {
            if (line.find(curr->first) != std::string::npos)
            {
                strSize = line.find(curr->first);
                std::string value = line.substr(strSize + curr->first.size() + 1);
                value.pop_back();
                (server.*(curr->second))(value);
            } 
            if (line.find("location") != std::string::npos)
            {
                setLocation(file, server);
            }
        }
    }
}

