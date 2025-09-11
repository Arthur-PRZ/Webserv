#ifndef _REQUESTMANAGEMENT_HPP_
#define _REQUESTMANAGEMENT_HPP_

#include <string>
#include <iostream>
#include <fstream>

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

    public :

    RequestManagement();
    ~RequestManagement();
    RequestManagement(const RequestManagement &copy);
    RequestManagement &operator=(const RequestManagement &other);
    
    void parser(std::string &request);
    void setBool();
    bool checkPath();

    std::string getMethod();
    std::string getPath();
    std::string getHttpVer();
    std::string getBody();
    bool getMethodFound();
    bool getPageFound();
    bool getGoodVer();
};

#endif