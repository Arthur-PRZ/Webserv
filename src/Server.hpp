#include <iostream>

class Server {
	private:
		std::string &host;
	public:
		Server();
		~Server();
		Server(const Server& other);
		Server &operator=(const Server& other);
};