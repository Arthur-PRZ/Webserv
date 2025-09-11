#include "SendManagement.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

SendManagement::SendManagement() : _response(""), _request() {}

SendManagement::SendManagement(RequestManagement request) : _response(""), _request(request) {}

SendManagement::~SendManagement() {}

void SendManagement::sendResponse(int client_fd) {
	ssize_t n = send(client_fd, _response.c_str(), _response.size(), 0);
	if (n == -1)
		throw std::runtime_error("send failed");
}

void SendManagement::CheckRequest() {
	if (_request.getPageFound())
		OK();
	else
		ErrorNotFound();
}

void SendManagement::OK() {
	std::ifstream file(_request.getPath().c_str(), std::ios::binary);
	std::string content;
	if (file) {
		content.assign((std::istreambuf_iterator<char>(file)),
		                    std::istreambuf_iterator<char>());
	}
	
	std::stringstream ss;
	ss << content.size();
	std::string content_length = ss.str();
	_response += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}

void SendManagement::ErrorNotFound() {
	std::ifstream ErrorPage(_request.getPath().c_str(), std::ios::binary);
	std::string content;
	if (ErrorPage) {
			content.assign((std::istreambuf_iterator<char>(ErrorPage)),
		                std::istreambuf_iterator<char>());
	}

	std::stringstream ss;
	ss << content.size();
	std::string content_length = ss.str();
	_response += "HTTP/1.1 404 NotFound\r\nContent-Type: text/html\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}
