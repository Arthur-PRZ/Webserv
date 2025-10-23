#include "RequestManagement.hpp"
#include "Location.hpp"
#include <cstddef>
#include <sstream>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <sys/socket.h>
#include <poll.h>
#include <string.h>

RequestManagement::RequestManagement()
    : _method(""), _path(""), _httpVer(""), _body(""), _contentType(""),
      _methodFound(false), _pageFound(false), _goodVer(false), _server(), _image() {}

RequestManagement::RequestManagement(Server server)
    : _method(""), _path(""), _httpVer(""), _body(""), _contentType(""),
      _methodFound(false), _pageFound(false), _goodVer(false), _server(server), _image() {}

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
		_server = other._server;
		_image = other._image;
    }
    return *this;
}

RequestManagement::RequestManagement(const RequestManagement &other)
        : _method(other._method), _path(other._path),
          _httpVer(other._httpVer), _body(other._body),
          _methodFound(other._methodFound),
          _pageFound(other._pageFound),
          _goodVer(other._goodVer),
		  _server(other._server),
		  _image(other._image)
{
}

unsigned long RequestManagement::toUnsignedLong(std::string &str)
{
    std::istringstream iss(str);
    unsigned long val = 0;

    iss >> val;
    return val;
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
			const std::vector<Location>& locations = _server.getLocations();

			for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
				std::cout << "La methode est " << it->getMethods() << " avec le path " << it->getPath() << " et le path " << _path << " et la _methode est " << _method << std::endl;
			    if (it->getPath() == word) {
					if (it->getMethods().find(_method) == std::string::npos) {
						_path = _server.getRoot() + "/505_error.html";
						std::cout << "test si ca rentre" << std::endl;
						break;
					}
					break ;
			    }
			}
			if (word == "/")
                _path = (_server.getRoot() + _server.getIndex()).c_str();
			else if (word.find(".py") != std::string::npos) {
			    const std::vector<Location>& locations = _server.getLocations();

			    for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
			        if (it->getPath() == "/cgi") {
						size_t pos = word.find("/", 2);
						word = word.substr(pos);
			            _path = (it->getRoot() + word).c_str();
			            break;
			        }
			    }
			}
			else
                _path = (_server.getRoot() + word).c_str();
        }
        else if ( i == 2 )
            _httpVer = word;
        i++;
    }
    setBool();
    setExtensionType();
	setContentType(request);
}

void RequestManagement::setContentType(std::string &request) {
    size_t pos = 0;
    size_t posBefore;
    size_t wordSize = 0;

    pos = request.find("Content-Type:");
    pos = request.find(' ', pos);
    pos++;
    posBefore = pos;

    wordSize = request.find(";", pos) - posBefore;
    _contentType = request.substr(posBefore, wordSize);
    pos = request.find("boundary");
    pos = request.find('=', pos);
    pos++;
    posBefore = pos;
    wordSize = request.find("\r\n", pos) - posBefore;
    std::string boundary = request.substr(posBefore, wordSize);
	if (_contentType == "multipart/form-data") {
		_image.parseImage(boundary, _body);
	}
}
bool RequestManagement::checkPath()
{
    if (!_path.empty() && _path[0] == '/')
        _path.erase(0, 1);

    struct stat sb;
    return stat(_path.c_str(), &sb) == 0;
}

void RequestManagement::setBool()
{
    if (_method.find("GET") != std::string::npos
        || _method.find("POST") != std::string::npos
        || _method.find("DELETE") != std::string::npos)
    {
        _methodFound = true;
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

void RequestManagement::setExtensionType()
{
    std::string line;
    size_t pos = 1;

    pos = _path.find('.', pos);
    if (pos != std::string::npos)
    {
        pos++;
        _extensionType = _path.substr(pos, _path.find('\0') - pos);
    }
    else
        _extensionType = "";

}
std::string &RequestManagement::getExtensionType()
{
    return _extensionType;
}

std::string &RequestManagement::getMethod()
{
    return _method;
}
std::string &RequestManagement::getPath()
{
    return _path;
}

std::string &RequestManagement::getHttpVer()
{
    return _httpVer;
}

std::string &RequestManagement::getBody()
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

Image RequestManagement::getImage() {
	return _image;
}

void RequestManagement::setClientBody(std::string body) {
	_body = body;
}
