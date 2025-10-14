#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <string>

enum ClientState {
	NONE,
	READING_HEADER,
	READING_BODY,
	PROCESS_REQUEST,
	SENDING_RESPONSE
};

class Client {
	private:
		ClientState state;
		int fd;
		int expectedBodySize;
		std::string request;
		std::string header;
		std::string body;

	public:
		Client(int socket);
		~Client();
		Client(const Client& other);
		Client &operator=(const Client& other);
};

#endif