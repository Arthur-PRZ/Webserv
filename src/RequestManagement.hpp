#ifndef _REQUESTMANAGEMENT_HPP_
#define _REQUESTMANAGEMENT_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include "Server.hpp"

class RequestManagement
{
    private :

    std::string _method;
    std::string _path;
    std::string _httpVer;
    std::string _body;
    bool _methodFound;
    bool _pageFound;
    bool _goodVer;
	Server _server;

    public :

	RequestManagement();
    RequestManagement(Server server);
    ~RequestManagement();
    RequestManagement(const RequestManagement &copy);
    RequestManagement &operator=(const RequestManagement &other);
    
    void parser(std::string &request);
    void setBool(std::string &request);
    void setBody(std::string &request);
    bool checkPath();

    static int toInt(std::string &str);

    std::string getMethod();
    std::string getPath();
    std::string getHttpVer();
    std::string getBody();
    bool getMethodFound();
    bool getPageFound();
    bool getGoodVer();
};

#endif