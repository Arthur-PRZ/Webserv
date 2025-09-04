#include <iostream>
#include <vector>
#include "Location.hpp"

class Server {
	private:
		std::string _host;
		int _port;
		std::string _serverName;
		std::string _root;
		std::string _index;
		std::string _errorPages;
		std::string _clientMaxBodySize;
		std::vector<Location> _locations;

	public:
		Server();
		~Server();
		Server(const Server& other);
		Server &operator=(const Server& other);
		int getPort( void ) const;
		const std::string &getHost( void ) const;
		const std::string &getServerName( void ) const;
		const std::string &getRoot( void ) const;
		const std::string &getIndex( void ) const;
		const std::string &getErrorPages( void ) const;
		const std::string &getClientMaxBodySize( void ) const;
		const std::vector<Location> &getLocations( void ) const;		
		void setPort( int port );
		void setHost( const std::string host );
		void setServerName( const std::string serverName );
		void setRoot( const std::string root );
		void setIndex( const std::string index );
		void setErrorPages( const std::string errorPages );
		void setClientMaxBodySize( const std::string clientMaxBodySize );
		void setLocations( const std::vector<Location>& locations );
};