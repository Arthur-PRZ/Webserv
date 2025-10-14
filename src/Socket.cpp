#include "Socket.hpp"
#include <sys/socket.h>
#include <stdio.h>

Socket::Socket() 
	: _fd(-1), _clientNbr(0){}

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
	return _clients;
}

int& Socket::getClientNbr()
{
	return _clientNbr;
}


void Socket::bind(int port) {
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt failed");

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("bind failed");
}


void Socket::listen() {
	::listen(_fd, SOMAXCONN);
	_clients[0].fd = _fd;
	_clients[0].events = POLLIN;
	_clientNbr++;
}

int Socket::accept() {
	int client_fd = ::accept(_fd, NULL, NULL);
	printf("accept returned fd=%d\n", client_fd);
	if (client_fd >= 0)
	{
		fcntl(client_fd, F_SETFL, O_NONBLOCK);
		_clients[_clientNbr].fd = client_fd;
		_clients[_clientNbr].events = POLLIN;
		_clientNbr++;
	}
	else
		throw("accept error");
	return client_fd;
}