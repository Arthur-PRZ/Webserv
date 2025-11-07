#include "SendManagement.hpp"
#include "Location.hpp"
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <sys/wait.h>
#include <vector>
#include <cstdio>

SendManagement::SendManagement() : _response(""), _request() , _server(){}

SendManagement::SendManagement(RequestManagement request, Server server) : _response(""), _request(request) , _server(server){}

SendManagement::~SendManagement() {}

bool SendManagement::sendResponse(int client_fd) {
	size_t total_sent = 0;
	while (total_sent < _response.size()) {
		ssize_t n = send(client_fd, _response.c_str() + total_sent,
		                     _response.size() - total_sent, 0);
		if (n <= 0) {
			return false;
		}
		total_sent += n;
	}
	return true;
} 

void SendManagement::checkRequest(std::string &extensionType) {
	if (_request.isMethodAuthorized() == false) {
			errorMethod();
			return ;
	}
	else if (_request.getMethod() == "GET") {
		std::vector<Location> vector = _server.getLocations();
		for (size_t i = 0; i < vector.size(); i++) {
			if (vector[i].getPath() == _request.getUrlPath() && vector[i].getAutoIndex() == true) {
                generateDirectoryListing();
                return;
            }
		}
		if (_request.getPageFound() || extensionType == "png" || extensionType == "txt") {
			OK(extensionType);
		}
		else
			errorNotFound();
	}
	else if (_request.getMethod() == "POST") {
	    if (_request.getUrlPath().find("/uploads") != 0) {
	        std::vector<Location> vector = _server.getLocations();
	        for (size_t i = 0; i < vector.size(); i++) {
	            if (vector[i].getPath() == "/cgi") {
	                if (vector[i].getCGIExtensions() != ".py") {
	                    errorNotFound();
	                    return ;
	                }
	            }
	        }
	        execPythonScript();
	    }
	    else {
	        std::string upload_path;
	        std::vector<Location> vector = _server.getLocations();

	        for (size_t i = 0; i < vector.size(); i++) {
	            if (vector[i].getPath() == "/uploads") {
	                upload_path = vector[i].getUploadsPath();
	            }
	        }
	        std::string path =  upload_path + "/" + _request.getImage().getFilename();
	        std::ofstream out(path.c_str(), std::ios::binary);
	        if (out) {
	            out.write(_request.getImage().getContent().c_str(), _request.getImage().getContent().size());
	            if (!out) {
	                std::cerr << "Erreur d'écriture dans le fichier\n";
	            }
	            out.close();
	        }
	        _response = "HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Length: 0\r\n\r\n";
	    }
	}
	else if (_request.getMethod() == "DELETE") {
		if (std::remove(_request.getFileToDelete().c_str()) == 0) {
			_response += "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n\r\n";
		} else {
			errorNotFound();
		}
	}
}

void SendManagement::generateDirectoryListing() {
    std::string urlPath = _request.getUrlPath();
    std::vector<Location> locations = _server.getLocations();
    std::string physicalPath;
    
    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i].getPath() == urlPath) {
            if (!locations[i].getRoot().empty()) {
                physicalPath = locations[i].getRoot();
            } else {
                physicalPath = _server.getRoot();
            }
            break;
        }
    }
    
    if (physicalPath.empty()) {
        physicalPath = _server.getRoot();
    }
    
    DIR *dir = opendir(physicalPath.c_str());
    if (!dir) {
        errorNotFound();
        return;
    }
    
    std::string html = "<!DOCTYPE html>\n";
    html += "<html>\n<head>\n";
    html += "<meta charset=\"UTF-8\">\n";
    html += "<title>Index of " + urlPath + "</title>\n";
    html += "<style>\n";
    html += "body { font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }\n";
    html += "h1 { color: #333; border-bottom: 2px solid #0066cc; padding-bottom: 10px; }\n";
    html += "hr { border: 1px solid #ddd; margin: 20px 0; }\n";
    html += "ul { list-style-type: none; padding: 0; background-color: white; border-radius: 5px; }\n";
    html += "li { padding: 12px 20px; border-bottom: 1px solid #eee; }\n";
    html += "li:last-child { border-bottom: none; }\n";
    html += "li:hover { background-color: #f0f8ff; }\n";
    html += "a { text-decoration: none; color: #0066cc; font-size: 16px; }\n";
    html += "a:hover { text-decoration: underline; color: #004499; }\n";
    html += ".folder { font-weight: bold; }\n";
    html += "</style>\n";
    html += "</head>\n<body>\n";
    html += "<h1>Index of " + urlPath + "</h1>\n";
    html += "<hr>\n<ul>\n";
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        
        if (name == "." || name == "..")
            continue;
        
        std::string fullPath = physicalPath;
        if (fullPath[fullPath.length() - 1] != '/')
            fullPath += "/";
        fullPath += name;
        
        struct stat statbuf;
        bool isDirectory = false;
        if (stat(fullPath.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            isDirectory = true;
        }
        
        std::string url = urlPath;
        if (url[url.length() - 1] != '/')
            url += "/";
        url += name;
        
        std::string displayName = name;
        std::string cssClass = "";
        if (isDirectory) {
            displayName += "/";
            cssClass = " class=\"folder\"";
        }
        
        html += "<li><a href=\"" + url + "\"" + cssClass + ">" + displayName + "</a></li>\n";
    }
    
    closedir(dir);
    
    html += "</ul>\n<hr>\n";
    html += "<p style=\"color: #888; font-size: 14px;\">Webserv/1.0</p>\n";
    html += "</body>\n</html>";
    
    std::stringstream ss;
    ss << html.size();
    std::string content_length = ss.str();
    
    _response = "HTTP/1.1 200 OK\r\n";
    _response += "Content-Type: text/html; charset=UTF-8\r\n";
    _response += "Content-Length: " + content_length + "\r\n";
    _response += "\r\n";
    _response += html;
}

void SendManagement::OK(std::string &extensionType) {
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
		if (extensionType == "png")
			type = "image/png";
		else if (extensionType == "txt")
			type = "plain/text";
	}
	std::stringstream ss;
    ss << content.size();
    std::string content_length = ss.str();
	content += _request.getImage().getContent();
	_response = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}

void SendManagement::errorMethod() {
	std::ifstream ErrorPage;
	std::map<int, std::string> error_pages = _server.getErrorPages();
	std::map<int, std::string>::iterator it;
	
	it = error_pages.find(405);
	if (it != error_pages.end()) {
		ErrorPage.open((_server.getRoot() + it->second).c_str(), std::ios::binary);
	} else {
		ErrorPage.open((_server.getRoot() + "/405_error.html").c_str(), std::ios::binary);
	}
	std::string content;
	if (ErrorPage) {
		content.assign((std::istreambuf_iterator<char>(ErrorPage)),
		               std::istreambuf_iterator<char>());
	}
	std::stringstream ss;
	ss << content.size();
	std::string content_length = ss.str();
	_response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}

void SendManagement::errorNotFound() {
	std::ifstream ErrorPage;
	std::map<int, std::string> error_pages = _server.getErrorPages();
	std::map<int, std::string>::iterator it;
	
	it = error_pages.find(404);
	if (it != error_pages.end()) {
		ErrorPage.open((_server.getRoot() + it->second).c_str(), std::ios::binary);
	} else {
		ErrorPage.open((_server.getRoot() + "/404_error.html").c_str(), std::ios::binary);
	}
	std::string content;
	if (ErrorPage) {
			content.assign((std::istreambuf_iterator<char>(ErrorPage)),
		                std::istreambuf_iterator<char>());
	}
	std::stringstream ss;
	ss << content.size();
	std::string content_length = ss.str();
	_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
}

void SendManagement::execPythonScript() {
	std::ostringstream oss;
	oss << _request.getBody().size();
	std::string contentLength = oss.str();
	std::string method = "REQUEST_METHOD=" + _request.getMethod();
	std::string length = "CONTENT_LENGTH=" + contentLength;
	std::string type = "CONTENT_TYPE=application/x-www-form-urlencoded";


	std::string fullPath = _request.getPath();
	std::string scriptPath;
	std::string queryString;

	size_t questionMarkPos = fullPath.find('?');
	if (questionMarkPos != std::string::npos) {
		scriptPath = fullPath.substr(0, questionMarkPos);
		queryString = fullPath.substr(questionMarkPos + 1);
	} else {
		scriptPath = fullPath;
		queryString = "";
	}

	std::string script = "SCRIPT_FILENAME=" + scriptPath;
	std::string query = "QUERY_STRING=" + queryString + ".txt";
	char *envp[] = {
		(char *)method.c_str(),
		(char *)length.c_str(),
		(char *)type.c_str(),
		(char *)script.c_str(),
		(char *)query.c_str(),
		NULL
	};

	int pipeIn[2];
	int pipeOut[2];
	pipe(pipeIn);
	pipe(pipeOut);
	char *argv[] = {
		(char *)"python3",
		(char *)scriptPath.c_str(),
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
		{
			if (write(pipeIn[1], _request.getBody().c_str(), _request.getBody().size()) <= 0)
    		{
				std::cout << "write failed" << std::endl;
				return ;
			}
		}
		close(pipeIn[1]);
		std::string cgiOutput;
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer))))
		{
			if (bytesRead <= 0)
			{
				std::cout << "read failed" << std::endl;
				return ;
			}
			cgiOutput.append(buffer, bytesRead);
		}
		close(pipeOut[0]);
		int status;
		waitpid(pid, &status, 0);

		std::string headers;
		std::string body;

		size_t pos = cgiOutput.find("\r\n\r\n");
		if (pos != std::string::npos) {
		    headers = cgiOutput.substr(0, pos);
		    body = cgiOutput.substr(pos + 4);
		} else {
		    pos = cgiOutput.find("\n\n");
		    if (pos != std::string::npos) {
		        headers = cgiOutput.substr(0, pos);
		        body = cgiOutput.substr(pos + 2);
		    } else {
		        body = cgiOutput;
		    }
		}

		std::string contentType = "text/html";
		std::istringstream headerStream(headers);
		std::string line;
		while (std::getline(headerStream, line)) {
		    if (line.find("Content-Type:") == 0) {
		        contentType = line.substr(13);
		        size_t start = contentType.find_first_not_of(" \t\r");
		        size_t end = contentType.find_last_not_of(" \t\r");
		        contentType = contentType.substr(start, end - start + 1);
		        break;
		    }
		}

		std::stringstream ss;
    	ss << cgiOutput.size();
    	std::string content_length = ss.str();
		_response = "HTTP/1.1 200 OK\r\n";
		_response += "Content-Type:" + contentType +"\r\n";
		_response += "Content-Length: " + content_length + "\r\n";
		_response += "\r\n";
		_response += cgiOutput;
	}
}

const std::string &SendManagement::getResponse() const {
	return _response;
}
