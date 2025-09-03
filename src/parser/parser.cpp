#include <iostream>
#include <string>
#include <fstream>

void parser(std::ifstream &file, Server &server)
{
    std::string line;
    size_t curr = 0;
    std::string tab[] = {"host" : setHost(), "server_name"}

    while (std::getline(file, line))
    {
        if (line.find(tab[i]) != std::string::npos)
        {
            curr = line.find(tab[i]);

            std::string value = line.substr(curr + 5);
            value.pop_back();
            server.setHost(value);
        }
    }

}