#include "RequestManagement.hpp"
#include <sstream>
#include <dirent.h>
#include <string>
#include <sys/stat.h>

RequestManagement::RequestManagement()
    : _method(""), _path(""), _httpVer(""), _body(""),
      _methodFound(false), _pageFound(false), _goodVer(false), _server() {}

RequestManagement::RequestManagement(Server server)
    : _method(""), _path(""), _httpVer(""), _body(""),
      _methodFound(false), _pageFound(false), _goodVer(false), _server(server) {}

RequestManagement::~RequestManagement() {}

RequestManagement &RequestManagement::operator=(const RequestManagement &other)
{
    if (this != &other)
    {
        _method = other._method;
        _path = other._path;
        _httpVer = other._httpVer;
        _body = other._body;
        _methodFound = other._methodFound;
        _pageFound = other._pageFound;
        _goodVer = other._goodVer;
    }
    return *this;
}

RequestManagement::RequestManagement(const RequestManagement &other)
        : _method(other._method), _path(other._path),
          _httpVer(other._httpVer), _body(other._body),
          _methodFound(other._methodFound),
          _pageFound(other._pageFound),
          _goodVer(other._goodVer)
{
}

int RequestManagement::toInt(std::string &str)
{
    std::istringstream iss(str);
    int val = 0;

    iss >> val;
    return val;
}

void RequestManagement::parser(std::string &request)
{
    std::string word;
    std::istringstream iss(request);
    int i = 0;

    while (iss >> word && i < 3)
    {
        if ( i == 0 )
            _method = word;
        else if ( i == 1 )
        {
            if (word == "/")
                _path = (_server.getRoot() + "/index.html").c_str();
            else
                _path = (_server.getRoot() + "/" + word).c_str();

        }
        else if ( i == 2 )
            _httpVer = word;
        i++;
    }
    setBool(request);
}

bool RequestManagement::checkPath()
{
    if (!_path.empty() && _path[0] == '/')
        _path.erase(0, 1);

    struct stat sb;
    return stat(_path.c_str(), &sb) == 0;
}

void RequestManagement::setBody(std::string &request)
{
    size_t pos = 0;
    size_t posBefore;
    size_t wordSize = 0;

    pos = request.find("Content-Length:");
    pos = request.find(' ', pos);
    pos++;
    posBefore = pos;
    
    wordSize = request.find("\r\n", pos) - posBefore;

    std::string lenght = request.substr(posBefore, wordSize);
    std::cout << lenght << " lenght" << std::endl;

    pos = request.find("username=", pos);
    _body = request.substr(pos, toInt(lenght));
    std::cout << _body << std::endl;
}

void RequestManagement::setBool(std::string &request)
{
    if (_method.find("GET") != std::string::npos
        || _method.find("POST") != std::string::npos
        || _method.find("DELETE") != std::string::npos)
    {
        _methodFound = true;
        if (_method.find("POST") != std::string::npos)
            setBody(request);
    }

    else
        _methodFound = false;
    if ((_path[0] == '/' && _path[1] == '\0') || checkPath() == true)
        _pageFound = true;
    else
        _pageFound = false;
    if (_httpVer.find("HTTP/1.1") != std::string::npos)
        _goodVer = true;
    else
        _goodVer = false;
}

std::string RequestManagement::getMethod()
{
    return _method;
}
std::string RequestManagement::getPath()
{
    return _path;
}

std::string RequestManagement::getHttpVer()
{
    return _httpVer;
}

std::string RequestManagement::getBody()
{
    return _body;
}

bool RequestManagement::getMethodFound()
{
    return _methodFound;
}

bool RequestManagement::getPageFound()
{
    return _pageFound;
}

bool RequestManagement::getGoodVer()
{
    return _goodVer;
}