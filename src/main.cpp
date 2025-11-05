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
#include <vector>
#include "RequestManagement.hpp"
#include "SendManagement.hpp"
#include "Server.hpp"
#include "Parser.hpp"
#include "Client.hpp"
#include "SignalsHandling.hpp"

//Objectif 05/11 -> Handle le CTRL + C, LEAK a faire.. et apres FINI :D

struct ServerContext {
	Socket *socket;
	Server *serverInfo;

	ServerContext() : socket(NULL), serverInfo(NULL) {}
	
	~ServerContext() {
		delete socket;
		delete serverInfo;
	}
};

int main(int argc, char **argv) {
	try {
		std::ifstream configFile("config.conf", std::ios::binary);
		if (argc == 2) {
			std::ifstream configFile(argv[1], std::ios::binary);
		}
		if (configFile.fail())
			configFile.open("config.conf", std::ios::binary);
		if (configFile.fail())
        	throw std::runtime_error("no config file found");
        
		SignalsHandling signalsHandler;
		signalsHandler.setSignals();

		std::vector<ServerContext*> serverList;
		int serverNbr = findServerNbr(configFile);
		
		std::cout << "Nombre de serveurs à créer: " << serverNbr << std::endl;
		configFile.clear();
		configFile.seekg(0, std::ios::beg);
		for (int i = 0; i < serverNbr; i++)
		{
			ServerContext *ctx = new ServerContext();
			ctx->socket = new Socket(AF_INET, SOCK_STREAM, 0);
			ctx->serverInfo = new Server();
			
			parserConfig(configFile, *(ctx->serverInfo));
			
			std::cout << "Configuration serveur " << i << " - Port: "
			          << ctx->serverInfo->getPort() << std::endl;
			if (ctx->serverInfo->getPort().empty()) {
				throw std::runtime_error("Port vide pour le serveur ");
			}

			ctx->socket->bind(RequestManagement::toInt(ctx->serverInfo->getPort()));
			ctx->socket->listen();
			
			std::cout << "Serveur " << i << " en écoute sur port " 
			          << ctx->serverInfo->getPort() << std::endl;
			
			serverList.push_back(ctx);
		}

		std::cout << "Tous les serveurs sont lancés !" << std::endl;

		while (signalsHandler.getStopStatus() == false) {
			for (size_t s = 0; s < serverList.size(); ++s) {
				ServerContext *ctx = serverList[s];
				Socket *server = ctx->socket;
				Server *serverInfo = ctx->serverInfo;
				
				int clientNbr = server->getClientNbr();
				if (clientNbr == 0)
					continue;
					
				pollfd *pollclients = server->getClients();
				
				int ret = poll(pollclients, clientNbr, 0);
				if (ret < 0)
				{	
					if (signalsHandler.getStopStatus() == true)
						break ;
					throw std::runtime_error("poll error");

				for (int i = clientNbr - 1; i >= 0; i--)
				{
					if (!(pollclients[i].revents & POLLIN))
						continue;
						
					if (pollclients[i].fd == server->getFd())
					{
						int new_fd = server->accept();
						serverInfo->addClient(new_fd);
						std::cout << "Nouveau client: " << new_fd 
								<< " sur port " << serverInfo->getPort() << std::endl;
					}
					else
					{
						int client_fd = pollclients[i].fd;
						Client &client = serverInfo->getClient(client_fd);

						char buffer[1024];
						int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
						if (bytes_received <= 0)
						{
							std::cout << "Client déconnecté: " << client_fd << std::endl;
							close(client_fd);
							serverInfo->removeClients(client_fd);
							server->removeClient(client_fd);
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
									pos = client.getRequest().find(" ", pos) + 1;
									size_t endPos = client.getRequest().find("\r\n", pos);
									std::string lengthStr = client.getRequest().substr(pos, endPos - pos);
									client.setExpectedBodySize(RequestManagement::toUnsignedLong(lengthStr));
									
									size_t bodyStart = client.getRequest().find("\r\n\r\n") + 4;
									client.setState(READING_BODY);

									if (client.getBody().size() >= (RequestManagement::toUnsignedLong(serverInfo->getClientMaxBodySize()) * 1024 * 1024)) {
										std::cout << "Client disconnected - body too large" << std::endl;
										close(client_fd);
										serverInfo->removeClients(client_fd);
										server->removeClient(i);
										continue;
									}
									
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
						}
						else if (client.getState() == READING_BODY) {
							std::string bodyChunck(buffer, bytes_received);

							client.setBody(bodyChunck);
							if (client.getBody().size() >= (RequestManagement::toUnsignedLong(serverInfo->getClientMaxBodySize()) * 1024 * 1024)) {
								std::cout << "Client disconnected - body too large" << std::endl;
								close(client_fd);
								serverInfo->removeClients(client_fd);
								server->removeClient(i);
								continue;
							}
							if (client.getBody().size() >= client.getExpectedBodySize()) {
								client.setState(PROCESS_REQUEST);
							}
						}
						if (client.getState() == PROCESS_REQUEST)
						{
							RequestManagement requestManagement(*serverInfo);
							requestManagement.setClientBody(client.getBody());
							requestManagement.parser(client.getRequest());
							
							SendManagement sendManagement(requestManagement, *serverInfo);
							sendManagement.checkRequest(requestManagement.getExtensionType());
							sendManagement.sendResponse(client_fd);
							client.reset();
							client.setState(READING_HEADER);
						}
					}
				}
			}
		}
	}
		for (size_t i = 0; i < serverList.size(); ++i) {
			delete serverList[i];
		}
		
	} catch (const std::exception& e) {
		std::cerr << "ERREUR: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
