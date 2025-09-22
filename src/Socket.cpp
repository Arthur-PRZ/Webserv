#include "Socket.hpp"
#include <sys/socket.h>

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

void Socket::bind(int port) {
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
}

int Socket::accept() {
	int client_fd = ::accept(_fd, NULL, NULL);
	if (client_fd >= 0)
		fcntl(client_fd, F_SETFL, O_NONBLOCK);
	return client_fd;
}