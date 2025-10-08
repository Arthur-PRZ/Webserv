#include "RequestManagement.hpp"
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

void RequestManagement::parser(std::string &request, pollfd &client)
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
                _path = (_server.getRoot() + word).c_str();

        }
        else if ( i == 2 )
            _httpVer = word;
        i++;
    }
    setBool(request, client);
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
	// std::cout << "request: "<< request << "|" << std::endl;
    // std::cout << "content type: " << _contentType << " |" << std::endl;
	if (_contentType == "multipart/form-data") {
        // std::cout << "in " << std::endl;
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

void RequestManagement::setBody(std::string &request, pollfd &client) 
{
    size_t pos = 0;
    size_t posBefore;
    size_t wordSize = 0;
	char buffer[4096];

    std::cout << "request " << request << " |" << std::endl;
    pos = request.find("Content-Length:");
    pos = request.find(' ', pos);
    pos++;
    posBefore = pos;
    
    wordSize = request.find("\r\n", pos) - posBefore;

    std::string length = request.substr(posBefore, wordSize);

    pos = request.find("\r\n\r\n", pos);
    pos += 4;

        while (_body.size() < toUnsignedLong(length) && client.revents & POLLIN) {
            int bytes_received = recv(client.fd, buffer, sizeof(buffer), 0);
            if (bytes_received == 0)
            {
                std::cout << "0" << std::endl;
                break;
            }
            else { // bytes == -1
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::cout << "-1 reçu : buffer vide pour l'instant (comportement normal)\n";
                } else {
                    std::cout << "Erreur réelle : " << strerror(errno) << "\n";
                }
            std::cout << "SIU" << std::endl;
            _body.append(buffer, bytes_received);
            }
        }
    std::cout << "lenght " << length << " |" << std::endl;
    std::cout << "body " << _body << " |" << std::endl;
}

void RequestManagement::setBool(std::string &request, pollfd &client)
{
    if (_method.find("GET") != std::string::npos
        || _method.find("POST") != std::string::npos
        || _method.find("DELETE") != std::string::npos)
    {
        _methodFound = true;
        if (_method.find("POST") != std::string::npos)
            setBody(request, client);
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
