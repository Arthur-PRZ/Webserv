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
		parserConfig(configFile, serverInfo);
	
		while (true) {
			pollfd *clients = server.getClients();
			std::string requests[10];

			// pollfd *clients = server.getClients();
		    // std::cout << server.getClientNbr() << std::endl;
			// std::string request;
			// int ret = poll(clients, server.getClientNbr(), -1);
			// if (ret > 0)
			// {
			// 	for (int i = 0; i < server.getClientNbr(); i++)
			// 	{
					
			// 		if (clients[i].revents & POLLIN)
			// 		{
			// 			char buffer[1024];
			// 			while (true) {
			// 				int bytes_received = recv(clients[i].fd, buffer, sizeof(buffer) - 1, 0);
			// 				if (bytes_received <= 0)
			// 					break;
			// 				buffer[bytes_received] = '\0';
			// 				request += buffer;
			// 				if (request.find("\r\n\r\n") != std::string::npos)
			// 					break;
			// 			}
			// 		}
			// 	}
		    // }


		    // std::cout << server.getClientNbr() << std::endl;

			std::string request;
			int ret = poll(clients, server.getClientNbr(), -1);
			if (ret < 0)
				throw("poll error");
			for (int i = 0; i < server.getClientNbr(); i++)
			{
				if (clients[i].fd == server.getFd() && clients[i].revents & POLLIN)
					server.accept();
				else if (clients[i].revents & POLLIN)
				{
					char buffer[1024];
					while (true) {
						//Client 5 -> Partie du body RECEIVING_BODY -> 200 octets, Client 12 GET index.html, Client 5 -> RECEIVING_BODY 200, Client 5 -> PROCESS_REQUEST, Client 5 -> SENDING_RESPONSE
						int bytes_received = recv(clients[i].fd, buffer, sizeof(buffer) - 1, 0);
						if (bytes_received <= 0)
							break;
						buffer[bytes_received] = '\0';
						requests[i] += buffer;
						if (requests[i].find("\r\n\r\n") != std::string::npos)
							break;
					}
					RequestManagement requestManagement(serverInfo);
					requestManagement.parser(requests[i] , clients[i]);
					SendManagement sendManagement(requestManagement, serverInfo);
					sendManagement.checkRequest(requestManagement.getExtensionType());
					sendManagement.sendResponse(clients[i].fd);
				}
			}
			}
		    // std::cout << request << std::endl;
		    // close(client_fd);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}