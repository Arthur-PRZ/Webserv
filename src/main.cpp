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

//Penser a gerer plusieurs clients en utilisant poll()
int main() {
	try {
		RequestManagement requestManagement;
		SendManagement sendManagement(requestManagement);
		Socket server(AF_INET, SOCK_STREAM, 0);
		server.bind(8080);
		server.listen();
		while (true) {
		    int client_fd = server.accept();

		    std::string request;
		    char buffer[1024];
		    while (true) {
		        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		        if (bytes_received <= 0)
					break;
		        buffer[bytes_received] = '\0';
		        request += buffer;
		        if (request.find("\r\n\r\n") != std::string::npos)
					break;
		    }
		    std::cout << request << std::endl;
			//Trouver GET et le chemin avec le contenu de request, Classe qui contient en BOOL -> Savoir si la page existe,
			//gere aussi si on a un type inconnu (Autre que POST GET ou DELETE), savoir si c'est bien en HTTP 1.1

			//Recup le chemin avec la std::string de la classe plus haut
			requestManagement.parser(request);
			sendManagement.CheckRequest();
		    std::ifstream file(requestManagement.getPath().c_str(), std::ios::binary);
		    std::string content;
		    if (file) {
		        content.assign((std::istreambuf_iterator<char>(file)),
		                       std::istreambuf_iterator<char>());
		    }

		    std::stringstream ss;
		    ss << content.size();
		    std::string content_length = ss.str();

			//Classe Header ou Response qui renvoie le Header/content en fonction de si la page existe ou pas, stocke aussi le body dans le cas d'une requete POST
			//Aussi la size de la reponse
		    /*
				Send.getResponse(); -> Renvoie une std::string avec la reponse
			*/
			std::string response =
		        "HTTP/1.1 200 OK\r\n"
		        "Content-Type: text/html\r\n"
		        "Content-Length: " + content_length + "\r\n"
		        "\r\n" + content;

		    size_t total_sent = 0;
		    while (total_sent < response.size()) {
		        ssize_t n = send(client_fd, response.c_str() + total_sent,
		                         response.size() - total_sent, 0);
		        if (n == -1) {
					throw std::runtime_error("send failed");
					break;
				}
		        total_sent += n;
		    }
		    close(client_fd);
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}