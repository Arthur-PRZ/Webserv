#ifndef _SendManagement_HPP_
#define _SendManagement_HPP_
#include <string>
#include "RequestManagement.hpp"
#include "Server.hpp"

class SendManagement {
	private:
		std::string _response;
		std::string _responseSize;
		RequestManagement _request;
		Server _server;

	public:
		SendManagement();
		SendManagement(RequestManagement request, Server server);
		~SendManagement();
		SendManagement(const SendManagement& other);
		SendManagement &operator=(const SendManagement& other);

		void sendResponse(int client_fd);
		const std::string &getResponse() const;
		int &getResponseSize();
		void checkRequest(std::string &extensionType);
		void OK(std::string &extensionType);
		void errorNotFound();
		void execPythonScript();
		
};

#endif

// DELETE ./cgi-bin/delete_ressource.py?ressource= + encoreURIComponent(ressource);
// GET index.html