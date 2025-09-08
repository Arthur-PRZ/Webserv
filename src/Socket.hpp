#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdexcept>
#include <unistd.h>

class Socket {
	private:
		int _fd;

	public:
		Socket();
		Socket(int domain, int type, int protocol);
		Socket(const Socket& other);
		Socket &operator=(const Socket& other);
		~Socket();

		int getFd() const;
		void bind(int port);
		void listen();
		int accept();
};

#endif