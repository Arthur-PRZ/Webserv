#include <cstddef>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <string>
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
		parserConfig(configFile, serverInfo);
		while (true) {
			pollfd *pollclients = server.getClients();
			int ret = poll(pollclients, server.getClientNbr(), -1);
			if (ret < 0)
				throw std::runtime_error("poll error");

			for (int i = 0; i < server.getClientNbr(); i++)
			{
				if (!(pollclients[i].revents & POLLIN))
					continue;
				if (pollclients[i].fd == server.getFd())
				{
					int new_fd = server.accept();
					serverInfo.addClient(new_fd);
					std::cout << "Nouveau client: " << new_fd << std::endl;
				}
				else
				{
					int client_fd = pollclients[i].fd;
					Client &client = serverInfo.getClient(client_fd);

					char buffer[1024];
					int bytes_received = recv(pollclients[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes_received <= 0)
					{
						close(client_fd);
						serverInfo.removeClients(client_fd);
						continue;
					}
					buffer[bytes_received] = '\0';
					if (client.getState() == READING_HEADER)
					{
						client.setRequest(buffer);
						if (client.getRequest().find("\r\n\r\n") != std::string::npos)
						{
							size_t pos = client.getRequest().find("Content-Length");
							if (pos != std::string::npos)
							{
								pos = client.getRequest().find(' ', pos) + 1;
							    size_t endPos = client.getRequest().find("\r\n", pos);
							    std::string lengthStr = client.getRequest().substr(pos, endPos - pos);
							    client.setExpectedBodySize(RequestManagement::toUnsignedLong(lengthStr));
							    
							    size_t bodyStart = client.getRequest().find("\r\n\r\n") + 4;
								client.setState(READING_BODY);

							    if (bodyStart < client.getRequest().size()) {
							        client.setBody(client.getRequest().substr(bodyStart));
							    }

							    if (client.getBody().size() >= client.getExpectedBodySize()) {
									client.setState(PROCESS_REQUEST);
							    }							
							} else {
								client.setState(PROCESS_REQUEST);
							}
						}
						std::cout << "The request is : " << client.getRequest() << std::endl;
					}
					else if (client.getState() == READING_BODY) {
						std::string bodyChunck(buffer, bytes_received);

						client.setBody(bodyChunck);
						if (client.getBody().size() >= client.getExpectedBodySize()) {
							client.setState(PROCESS_REQUEST);
						}
					}

					if (client.getState() == PROCESS_REQUEST)
					{
						RequestManagement requestManagement(serverInfo);
						requestManagement.setClientBody(client.getBody());
						requestManagement.parser(client.getRequest());
						
						SendManagement sendManagement(requestManagement, serverInfo);
						sendManagement.checkRequest(requestManagement.getExtensionType());
						sendManagement.sendResponse(client_fd);
						client.reset();
						client.setState(READING_HEADER);
					}
				}
			}
			}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}