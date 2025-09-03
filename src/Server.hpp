#include <iostream>
#include <vector>
#include "Location.hpp"

class Server {
	private:
		std::string host;
		int port;
		std::string serverName;
		std::string root;
		std::string index;
		std::string errorPages;
		std::string clientMaxBodySize;
		std::vector<Location> locations;

	public:
		Server();
		~Server();
		Server(const Server& other);
		Server &operator=(const Server& other);
		int getPort( void );
		std::string getHost( void );
		std::string getServerName( void );
		std::string getRoot( void );
		std::string getIndex( void );
		std::string getErrorPages( void );
		std::string getClientMaxBodySize( void );
		std::vector<Location> getLocations( void );		
		int setPort( void );
		std::string setHost( void );
		std::string setServerName( void );
		std::string setRoot( void );
		std::string setIndex( void );
		std::string setErrorPages( void );
		std::string setClientMaxBodySize( void );
		std::vector<Location> setLocations( void );
};