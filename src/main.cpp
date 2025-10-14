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
#include "Client.hpp"

//Penser a gerer plusieurs pollclients en utilisant poll()
int main(int argc, char **argv) {
	try {
		(void)argc;
		Socket server(AF_INET, SOCK_STREAM, 0);
		Server serverInfo;
		server.bind(8080);
		server.listen();

		std::ifstream configFile(argv[1], std::ios::binary);
		if (configFile.fail())
			configFile.open("config.conf", std::ios::binary);
		if (configFile.fail())
        	throw std::runtime_error("no config file found");
		server.accept();
		parserConfig(configFile, serverInfo);
		while (true) {
			Client client;
			pollfd *pollclients = server.getClients();
			int ret = poll(pollclients, server.getClientNbr(), -1);
			if (ret < 0)
				throw("poll error");
			for (int i = 0; i < server.getClientNbr(); i++)
			{
				if (pollclients[i].fd == server.getFd() && pollclients[i].revents & POLLIN)
				{
					int new_fd = server.accept();
					client = serverInfo.addClient(new_fd);
				}
				else if (pollclients[i].revents & POLLIN)
				{
					char buffer[1024];
					if (client.getState() == READING_HEADER)
					{
						int bytes_received = recv(pollclients[i].fd, buffer, sizeof(buffer) - 1, 0);
						if (bytes_received <= 0)
							break;
						buffer[bytes_received] = '\0';
						client.setRequest(buffer);
						if (client.getRequest().find("\r\n\r\n") != std::string::npos)
							break;
					}
					RequestManagement requestManagement(serverInfo);
					requestManagement.parser(client.getRequest() , pollclients[i]);
					SendManagement sendManagement(requestManagement, serverInfo);
					sendManagement.checkRequest(requestManagement.getExtensionType());
					sendManagement.sendResponse(pollclients[i].fd);
				}
			}
			}
		    // std::cout << request << std::endl;
		    // close(client_fd);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}