#include <exception>
#include <iterator>
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
#include <fstream>
#include <sstream>
#include "RequestManagement.hpp"
#include "SendManagement.hpp"
#include "Server.hpp"
#include "Parser.hpp"

//Penser a gerer plusieurs clients en utilisant poll()
int main() {
	try {
		Socket server(AF_INET, SOCK_STREAM, 0);
		Server serverInfo;
		server.bind(8080);
		server.listen();
		std::ifstream configFile("config.conf", std::ios::binary);
		while (true) {
		    int client_fd = server.accept();
			pollfd *clients = server.getClients();
			std::string request;

			int ret = poll(clients, server.getClientNbr(), -1);

			if (ret > 0)
			{
				for (int i = 0; i < server.getClientNbr(); i++)
				{
					
					if (clients[i].revents & POLLIN)
					{
						char buffer[1024];
						while (true) {
							int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
							std::cout << bytes_received << std::endl;

							if (bytes_received <= 0)
								break;
							buffer[bytes_received] = '\0';
							request += buffer;
							if (request.find("\r\n\r\n") != std::string::npos)
								break;
						}
					}
				}				
		    }
		    std::cout << request << std::endl;

			parserConfig(configFile, serverInfo);
			RequestManagement requestManagement(serverInfo);
			requestManagement.parser(request);
			SendManagement sendManagement(requestManagement, serverInfo);
			sendManagement.checkRequest();
			std::string filePath = requestManagement.getPath();
		    std::ifstream file(filePath.c_str(), std::ios::binary);
		    std::string content;
		    if (file) {
		        content.assign((std::istreambuf_iterator<char>(file)),
		                       std::istreambuf_iterator<char>());
		    }

		    std::stringstream ss;
		    ss << content.size();
		    std::string content_length = ss.str();

			sendManagement.sendResponse(client_fd);
		    close(client_fd);
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}