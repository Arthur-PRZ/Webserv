#include "Socket.hpp"
#include <sys/socket.h>
#include <stdio.h>

Socket::Socket() 
	: _fd(-1), _clientNbr(0){
	}

Socket::Socket(const Socket& other) {
	_fd = other._fd;
	_clientNbr = other._clientNbr;
	for (int i = 0; i < _clientNbr; i++)
		_clients[i] = other._clients[i];
}

Socket &Socket::operator=(const Socket& other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_clientNbr = other._clientNbr;
		for (int i = 0; i < _clientNbr; i++)
			_clients[i] = other._clients[i];
	}
	return *this;
}

Socket::Socket(int domain, int type, int protocol) : _clientNbr(0) {
	_fd = socket(domain, type, protocol);
	if (_fd == -1)
		throw std::runtime_error("socket failed");
}

Socket::~Socket() {
	if (_fd != -1)
		close(_fd);
}

int Socket::getFd() const {
	return _fd;
}

pollfd* Socket::getClients()
{
	return &_clients[0];
}

int Socket::getClientNbr()
{
	return _clients.size();
}

int Socket::bind(int port) {
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        return -1;

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
	if (::bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		return -1;
	return 0;
}

void Socket::removeClient(int fd) {
    for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i].fd == fd) {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}

void Socket::listen() {
	::listen(_fd, SOMAXCONN);
	pollfd server_pollfd;
	server_pollfd.fd = _fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	
	_clients.push_back(server_pollfd);
}

int Socket::accept() {
	int client_fd = ::accept(_fd, NULL, NULL);
	printf("accept returned fd=%d\n", client_fd);
	if (client_fd >= 0)
	{
		fcntl(client_fd, F_SETFL, O_NONBLOCK);
		pollfd client_pollfd;
		client_pollfd.fd = client_fd;
		client_pollfd.events = POLLIN;
		client_pollfd.revents = 0;
		
		_clients.push_back(client_pollfd);
	}
	else
		throw("accept error");
	return client_fd;
}