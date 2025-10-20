#include "Client.hpp"

Client::Client() : _state(READING_HEADER), _fd(-1), _expectedBodySize(0),
							_request(""), _header(""), _body("") {}

Client::Client(int socket) : _state(READING_HEADER), _fd(socket), _expectedBodySize(0),
							_request(""), _header(""), _body("") {}
Client::~Client() {}

Client::Client(const Client& other) : _state(other._state), _fd(other._fd), _expectedBodySize(other._expectedBodySize),
							_request(other._request), _header(other._header), _body(other._body) {}

Client &Client::operator=(const Client& other) {
	if (this != &other) {
		this->_state = other._state;
		this->_fd = other._fd;
		this->_expectedBodySize = other._expectedBodySize;
		this->_request = other._request;
		this->_header = other._header;
		this->_body = other._body;
	}
	return *this;
}

void Client::setRequest( char *buffer ) {
	_request += buffer; 
}

void Client::setState( ClientState state ) {
	_state = state;
}

void Client::setFd( int fd ) {
	_fd = fd;
}

void Client::setExpectedBodySize( int bodySize ) {
	_expectedBodySize = bodySize;
}

void Client::setHeader( std::string &header ) {
	_header += header;
}

void Client::setBody( std::string body ) {
	_body += body;
}

std::string &Client::getRequest() { 
	return _request;
}

ClientState &Client::getState() {
	return _state;
}

int &Client::getFd() {
	return _fd;
}

unsigned long &Client::getExpectedBodySize() {
	return _expectedBodySize;
}

std::string &Client::getHeader() {
	return _header;
}

std::string &Client::getBody() {
	return _body;
}

void Client::reset() {
	_request = "";
	_body = "";
	_header = "";
	_expectedBodySize = 0;
}