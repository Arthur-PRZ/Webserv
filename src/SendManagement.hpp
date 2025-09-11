#ifndef _SendManagement_HPP_
#define _SendManagement_HPP_
#include <string>
#include "RequestManagement.hpp"

class SendManagement {
	private:
		std::string _response;
		std::string _responseSize;
		RequestManagement _request;

	public:
		SendManagement();
		SendManagement(RequestManagement request);
		~SendManagement();
		SendManagement(const SendManagement& other);
		SendManagement &operator=(const SendManagement& other);

		void sendResponse(int client_fd);
		const std::string &getResponse() const;
		int &getResponseSize();
		void CheckRequest();
		void OK();
		void ErrorNotFound();
};

#endif