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
    : _method(""), _path(""), _httpVer(""), _body(""), _contentType(""), _urlPath(""), _fileToDelete(""),
      _methodFound(false), _pageFound(false), _goodVer(false), _server(), _image() {}

RequestManagement::RequestManagement(Server server)
    : _method(""), _path(""), _httpVer(""), _body(""), _contentType(""), _urlPath(""), _fileToDelete(""),
      _methodFound(false), _pageFound(false), _goodVer(false), _authorizedMethod(true), _server(server), _image() {}

RequestManagement::~RequestManagement() {}

RequestManagement &RequestManagement::operator=(const RequestManagement &other)
{
    if (this != &other)
    {
        _method = other._method;
        _path = other._path;
        _httpVer = other._httpVer;
        _body = other._body;
		_contentType = other._contentType;
		_urlPath = other._urlPath;
		_fileToDelete = other._fileToDelete;
        _methodFound = other._methodFound;
        _pageFound = other._pageFound;
        _goodVer = other._goodVer;
		_authorizedMethod = other._authorizedMethod;
		_server = other._server;
		_image = other._image;
    }
    return *this;
}

RequestManagement::RequestManagement(const RequestManagement &other)
        : _method(other._method), _path(other._path),
          _httpVer(other._httpVer), _body(other._body),
		  _contentType(other._contentType), _urlPath(other._urlPath),
		  _fileToDelete(other._fileToDelete),
          _methodFound(other._methodFound),
          _pageFound(other._pageFound),
          _goodVer(other._goodVer),
		  _authorizedMethod(other._authorizedMethod),
		  _server(other._server),
		  _image(other._image)
{
}

Location* RequestManagement::findMatchingLocation(const std::string &urlPath) {
    const std::vector<Location>& locations = _server.getLocations();
    Location* bestMatch = NULL;
    size_t longestMatch = 0;
    
    for (size_t i = 0; i < locations.size(); i++) {
        std::string locPath = locations[i].getPath();
        if (urlPath.find(locPath) == 0) {
            if (urlPath.length() == locPath.length() || urlPath[locPath.length()] == '/' || urlPath[locPath.length()] == '?') {
				if (locPath.length() > longestMatch) {
					bestMatch = const_cast<Location*>(&locations[i]);
					longestMatch = locPath.length();
				}
			}
		}
	}
    return bestMatch;
}

std::string RequestManagement::buildPhysicalPath(const std::string &urlPath, Location *location) {
    std::string physicalPath;
    std::string locPath = location->getPath();
    
    std::string remainder;
    if (urlPath.length() > locPath.length()) {
        remainder = urlPath.substr(locPath.length());
        if (!remainder.empty() && remainder[0] == '/') {
            remainder = remainder.substr(1);
        }
    }
    std::string pathPart = remainder;
    size_t queryPos = remainder.find('?');
    if (queryPos != std::string::npos) {
        pathPart = remainder.substr(0, queryPos);
    }
    
	if (!location->getUploadsPath().empty()) {
		physicalPath = location->getUploadsPath();
	} else if (!location->getRoot().empty()) {
        physicalPath = location->getRoot();
    } else {
        physicalPath = _server.getRoot();
    }
    
    if (!pathPart.empty()) {
        if (physicalPath[physicalPath.length() - 1] != '/') {
            physicalPath += "/";
        }
        physicalPath += pathPart;
    } else {
        if (urlPath == locPath) {
            if (urlPath == "/") {
                std::string index = _server.getIndex();
                if (!index.empty() && index[0] == '/') {
                    index = index.substr(1);
                }
                if (physicalPath[physicalPath.length() - 1] != '/') {
                    physicalPath += "/";
                }
                physicalPath += index;
            }
        }
    }
	_fileToDelete = physicalPath;
    if (queryPos != std::string::npos) {
        physicalPath += "?" + remainder.substr(queryPos + 1);
    }
	
    return physicalPath;
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
        if (i == 0) {
            _method = word;
        }
        else if (i == 1) {
            _urlPath = word;
            
            Location *matchedLocation = findMatchingLocation(_urlPath);
			if (matchedLocation) {
				if (matchedLocation->getMethods().find(_method) == std::string::npos) {
				    _authorizedMethod = false;
				} else {
				    _authorizedMethod = true;
				}
				_path = buildPhysicalPath(_urlPath, matchedLocation);
            } else {
                if (_method == "GET") {
                    _authorizedMethod = true;
                    _path = _server.getRoot();
                    if (_urlPath != "/") {
                        std::string urlCopy = _urlPath;
                        if (urlCopy[0] == '/') {
                            urlCopy = urlCopy.substr(1);
                        }
                        if (_path[_path.length() - 1] != '/') {
                            _path += "/";
                        }
                        _path += urlCopy;
                    } else {
                        std::string index = _server.getIndex();
                        if (!index.empty() && index[0] == '/') {
                            index = index.substr(1);
                        }
                        if (_path[_path.length() - 1] != '/') {
                            _path += "/";
                        }
                        _path += index;
                    }
                } else {
                    _authorizedMethod = false;
                    _path = _server.getRoot() + _urlPath;
                }
            }
        }
        else if (i == 2) {
            _httpVer = word;
        }
        i++;
    }
    
    setBool();
    setExtensionType();
    setContentType(request);
}

std::string &RequestManagement::getUrlPath() {
    return _urlPath;
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

bool RequestManagement::isMethodAuthorized() {
	return _authorizedMethod;
}

std::string &RequestManagement::getFileToDelete() {
	return _fileToDelete;
}
