#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <string>

enum ClientState {
	READING_HEADER,
	READING_BODY,
	PROCESS_REQUEST,
	SENDING_RESPONSE
};

class Client {
	private:
		ClientState _state;
		int _fd;
		int _expectedBodySize;
		std::string _request;
		std::string _header;
		std::string _body;

	public:
		Client();
		Client(int socket);
		~Client();
		Client(const Client& other);
		Client &operator=(const Client& other);

		void setRequest( char *buffer );
		void setState( ClientState state );
		void setFd( int fd );
		void setExpectedBodySize( int bodySize );
		void setHeader( std::string &header );
		void setBody( std::string &body );

		std::string &getRequest();
		ClientState &getState();
		int &getFd();
		int &getExpectedBodySize();
		std::string &getHeader();
		std::string &getBody();
};

#endif