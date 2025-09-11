#include "RequestManagement.hpp"
#include <sstream>

RequestManagement::RequestManagement()
    : _method(""), _path(""), _httpVer(""), _body(""),
      _methodFound(false), _pageFound(false), _goodVer(false)
{
}

RequestManagement::~RequestManagement()
{
}

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
				_path = "www/index.html";
			else
            	_path = word;
		}
        else if ( i == 2 )
            _httpVer = word;
        i++;
    }
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