#include "Server.hpp"

Server::Server()
	: _host(""),
	  _port(""),
	  _serverName(""),
	  _root(""),
	  _index(""),
	  _errorPages(""),
	  _clientMaxBodySize(""),
	  _locations() {}

Server::~Server() {}

Server::Server(const Server& other)
	: _host(other._host),
	  _port(other._port),
	  _serverName(other._serverName),
	  _root(other._root),
	  _index(other._index),
	  _errorPages(other._errorPages),
	  _clientMaxBodySize(other._clientMaxBodySize),
	  _locations(other._locations) {}

Server &Server::operator=(const Server& other) {
	if (this != &other)
	{
		_host = other._host;
		_port = other._port;
		_root = other._root;
		_index = other._index;
		_errorPages = other._errorPages;
		_clientMaxBodySize = other._clientMaxBodySize;
		_locations = other._locations; 
	}
	return *this;
}

const std::string &Server::getPort() const {
	return _port;
}

const std::string &Server::getHost() const {
	return _host;
}

const std::string &Server::getServerName() const {
	return _serverName;
}

const std::string &Server::getRoot() const {
	return _root;
}

const std::string &Server::getIndex() const {
	return _index;
}

const std::string &Server::getErrorPages() const {
	return _errorPages;
}

const std::string &Server::getClientMaxBodySize() const {
	return _clientMaxBodySize;
}

const std::vector<Location> &Server::getLocations() const {
	return _locations;
}

void Server::setPort( const std::string& port ) {
	_port = port;
}

void Server::setHost( const std::string& host ) {
	_host = host;
}

void Server::setServerName( const std::string& serverName ) {
	_serverName = serverName;
}

void Server::setRoot( const std::string& root ) {
	_root = root;
}

void Server::setIndex( const std::string& index ) {
	_index = index;
}

void Server::setErrorPages( const std::string& errorPages ) {
	_errorPages = errorPages;
}

void Server::setClientMaxBodySize( const std::string& clientMaxBodySize ) {
	_clientMaxBodySize = clientMaxBodySize;
}

void Server::setLocations( const std::vector<Location>& locations ) {
	_locations = locations;
}

void Server::addLocation( const Location &location ) 
{
	_locations.push_back(location);
}
