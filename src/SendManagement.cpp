#include "SendManagement.hpp"
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <sys/wait.h>

SendManagement::SendManagement() : _response(""), _request() , _server(){}

SendManagement::SendManagement(RequestManagement request, Server server) : _response(""), _request(request) , _server(server){}

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

void SendManagement::checkRequest(std::string &extensionType) {
	if (_request.getMethod() == "GET") {
		if (_request.getPageFound() || extensionType == "png")
			OK();
		else
			errorNotFound();
	}
	else if (_request.getMethod() == "POST") {
		if (_request.getPath() != "./www/upload")
			execPythonScript();
		else
		{
			std::string path = "./www/uploads/" + _request.getImage().getFilename();
			std::ofstream out(path.c_str(), std::ios::binary);
			if (out) {
			    out.write(_request.getImage().getContent().c_str(), _request.getImage().getContent().size());
			    out.close();
			}
			_response = "HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Length: 0\r\n\r\n";
		}
	}
}

void SendManagement::OK() {
	std::string content;
	std::string type;
	if (_request.getPageFound())
	{
		std::string filePath = _request.getPath();
		std::ifstream file(filePath.c_str(), std::ios::binary);
		if (file) {
	        content.assign((std::istreambuf_iterator<char>(file)),
		                       std::istreambuf_iterator<char>());
		}
		else
			errorNotFound();
		type = "text/html";
	}
	else
	{
		std::string filePath = _request.getPath();
		std::ifstream file(filePath.c_str(), std::ios::binary);
		if (file) {
	        content.assign((std::istreambuf_iterator<char>(file)),
		                       std::istreambuf_iterator<char>());
		}
		else
			errorNotFound();
		type = "image/png";
	}
	std::stringstream ss;
    ss << content.size();
    std::string content_length = ss.str();
	content += _request.getImage().getContent();
	_response = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}

void SendManagement::errorNotFound() {
	std::ifstream ErrorPage((_server.getRoot() + "/404_error.html").c_str(), std::ios::binary);
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

void SendManagement::execPythonScript() {
	std::ostringstream oss;
	oss << _request.getBody().size();
	std::string contentLength = oss.str();
	std::string method = "REQUEST_METHOD=" + _request.getMethod();
	std::string length = "CONTENT_LENGTH=" + contentLength;
	std::string type = "CONTENT_TYPE=application/x-www-form-urlencoded";
	std::string script = "SCRIPT_FILENAME=./cgi-bin/hello.py";

	char *envp[] = {
		(char *)method.c_str(),
		(char *)length.c_str(),
		(char *)type.c_str(),
		(char *)script.c_str(),
		NULL
	};

	int pipeIn[2];
	int pipeOut[2];
	pipe(pipeIn);
	pipe(pipeOut);
	char *argv[] = {
		(char *)"python3",
		(char *)"./cgi-bin/hello.py",
		NULL
	};

	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("fork failed");
	if (pid == 0) {
		close(pipeIn[1]);
		close(pipeOut[0]);
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);
		execve("/usr/bin/python3", argv, envp);
		kill(getpid(), SIGKILL);
	}
	else if (pid > 0) {
		close(pipeIn[0]);
		close(pipeOut[1]);
		if (!_request.getBody().empty())
			write(pipeIn[1], _request.getBody().c_str(), _request.getBody().size());
		close(pipeIn[1]);
		std::string cgiOutput;
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer))) > 0)
			cgiOutput.append(buffer, bytesRead);
		close(pipeOut[0]);
		int status;
		waitpid(pid, &status, 0);
		_response += cgiOutput;
	}
}
