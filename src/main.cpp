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
#include "Socket.hpp"

int main() {
	try {
		Socket server(AF_INET, SOCK_STREAM, 0);
		server.bind(8080);
		server.listen();
		while (true)
		{
			int client_fd = server.accept();
			int file_fd = open("index.html", O_RDONLY);
			if (file_fd == -1) {
				const char* not_found = 
					"HTTP/1.1 404 Not Found\r\n"
					"Content-Length: 0\r\n"
					"\r\n";
				send(client_fd, not_found, strlen(not_found), 0);
				close(client_fd);
				return 1;
			}
		
			struct stat st;
			if (fstat(file_fd, &st) == -1) {
				close(file_fd);
				close(client_fd);
				return 1;
			}
			size_t file_size = st.st_size;
		
			char* buffer = new char[file_size];
			ssize_t bytes_read = read(file_fd, buffer, file_size);
			close(file_fd);
		
			if (bytes_read <= 0) bytes_read = 0;
		
			const char* headers1 = "HTTP/1.1 200 OK\r\n";
			const char* headers2 = "Content-Type: text/html\r\n";
		
			char content_length[64] = "Content-Length: ";
			char num[20];
			int len = 0, size = file_size;
			if (size == 0) num[len++] = '0';
			while (size > 0) { num[len++] = '0' + (size % 10); size /= 10; }
			for (int i = 0; i < len/2; ++i) { char tmp=num[i]; num[i]=num[len-1-i]; num[len-1-i]=tmp; }
			std::memcpy(content_length + 16, num, len);
			std::memcpy(content_length + 16 + len, "\r\n\r\n", 4);
			send(client_fd, headers1, strlen(headers1), 0);
			send(client_fd, headers2, strlen(headers2), 0);
			send(client_fd, content_length, 16+len+4, 0);
			send(client_fd, buffer, bytes_read, 0);
			delete[] buffer;
			close(client_fd);
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}