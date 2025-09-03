#include "Server.hpp"

Server::Server(): host("Unknown"), port(0), serverName("Unknown"), root("Unknown"), index("Unknown"), errorPages("Unknown"), clientMaxBodySize("0M"), locations(0) {}

Server::~Server() {}

Server::Server(const Server& other): host(other.host), port(other.port), serverName(other.serverName), root(other.root), index(other.index), errorPages(other.errorPages), clientMaxBodySize(other.clientMaxBodySize), locations(other.locations) {}

Server &Server::operator=(const Server& other) {
	if (this != &other)
	{}
	return *this;
}

std::string Server::getHost() {
	return this->host;
}
