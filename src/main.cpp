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

int main() {
	try {
		Socket server(AF_INET, SOCK_STREAM, 0);
		server.bind(8080);
		server.listen();
		while (true) {
		    int client_fd = server.accept();

		    // --- Lire la requête HTTP ---
		    std::string request;
		    char buffer[1024];
		    while (true) {
		        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		        if (bytes_received <= 0) break;
		        buffer[bytes_received] = '\0';
		        request += buffer;
		        if (request.find("\r\n\r\n") != std::string::npos) break;
		    }
		    std::cout << request << std::endl;

		    // --- Lire le fichier HTML ---
		    std::ifstream file("www/index.html", std::ios::binary);
		    std::string content;
		    if (file) {
		        content.assign((std::istreambuf_iterator<char>(file)),
		                       std::istreambuf_iterator<char>());
		    }

		    // --- Préparer la réponse HTTP ---
		    std::stringstream ss;
		    ss << content.size();
		    std::string content_length = ss.str();

		    std::string response =
		        "HTTP/1.1 200 OK\r\n"
		        "Content-Type: text/html\r\n"
		        "Content-Length: " + content_length + "\r\n"
		        "\r\n" + content;

		    // --- Envoyer toute la réponse ---
		    size_t total_sent = 0;
		    while (total_sent < response.size()) {
		        ssize_t n = send(client_fd, response.c_str() + total_sent,
		                         response.size() - total_sent, 0);
		        if (n == -1) { perror("send failed"); break; }
		        total_sent += n;
		    }

		    close(client_fd);
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}