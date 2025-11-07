#ifndef _SendManagement_HPP_
#define _SendManagement_HPP_
#include <string>
#include "RequestManagement.hpp"
#include "Server.hpp"
#include <dirent.h>
#include <sys/stat.h>

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

		bool sendResponse(int client_fd);
		const std::string &getResponse() const;
		int &getResponseSize();
		void checkRequest(std::string &extensionType);
		void OK(std::string &extensionType);
		void errorNotFound();
		void execPythonScript();
		void errorMethod();
		void generateDirectoryListing();
};

#endif
