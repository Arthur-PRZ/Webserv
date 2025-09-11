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
	size_t total_sent = 0;
	while (total_sent < _response.size()) {
		ssize_t n = send(client_fd, _response.c_str() + total_sent,
		                     _response.size() - total_sent, 0);
		if (n == -1) {
			throw std::runtime_error("send failed");
		}
		total_sent += n;
	}
}

void SendManagement::CheckRequest() {
	if (_request.getPageFound())
		OK();
	else
		ErrorNotFound();
}

void SendManagement::OK() {
	std::string filePath = _request.getPath();
	std::ifstream file(filePath.c_str(), std::ios::binary);
	std::string content;
	if (file) {
        content.assign((std::istreambuf_iterator<char>(file)),
	                       std::istreambuf_iterator<char>());
	}

	std::stringstream ss;
    ss << content.size();
    std::string content_length = ss.str();
	_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}

void SendManagement::ErrorNotFound() {
	std::ifstream ErrorPage("www/404_error.html", std::ios::binary);
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
