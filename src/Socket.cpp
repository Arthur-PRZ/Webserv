#include "Socket.hpp"
#include <sys/socket.h>

Socket::Socket() 
	: _fd(-1) {}

Socket::Socket(int domain, int type, int protocol) {
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
		throw std::runtime_error("socket failed");
}

void Socket::listen() {
	::listen(_fd, SOMAXCONN);
}

int Socket::accept() {
	int client_fd = ::accept(_fd, NULL, NULL);
	return client_fd;
}