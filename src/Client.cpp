#include "Client.hpp"

Client::Client(int socket) : state(NONE), fd(socket), expectedBodySize(0),
							request(""), header(""), body("") {}
Client::~Client() {}

Client::Client(const Client& other) : state(other.state), fd(other.fd), expectedBodySize(other.expectedBodySize),
							request(other.request), header(other.header), body(other.body) {}

Client &Client::operator=(const Client& other) {
	if (this != &other) {
		this->state = other.state;
		this->fd = other.fd;
		this->expectedBodySize = other.expectedBodySize;
		this->request = other.request;
		this->header = other.header;
		this->body = other.body;
	}
	return *this;
}