#ifndef _SERVER_HPP_
#define _SERVER_HPP_
#include <string>
#include <vector>
#include "Location.hpp"
#include "Client.hpp"
#include <map>

class Server {
	private:
		std::string _host;
		std::string _port;
		std::string _serverName;
		std::string _root;
		std::string _index;
		std::string _errorPages;
		std::string _clientMaxBodySize;
		std::vector<Location> _locations;
		std::map<int, Client> _clients;

	public:
		Server();
		~Server();
		Server(const Server& other);
		Server &operator=(const Server& other);

		std::string &getPort( void );
		const std::string &getHost( void ) const;
		const std::string &getServerName( void ) const;
		const std::string &getRoot( void ) const;
		const std::string &getIndex( void ) const;
		std::string &getErrorPages( void );
		std::string &getClientMaxBodySize( void );
		const std::vector<Location> &getLocations( void ) const;
	
		void setPort( const std::string& port );
		void setHost( const std::string& host );
		void setServerName( const std::string& serverName );
		void setRoot( const std::string& root );
		void setIndex( const std::string& index );
		void setErrorPages( const std::string& errorPages );
		void setClientMaxBodySize( const std::string& clientMaxBodySize );
		void setLocationsServer( const std::vector<Location>& locations );

		void addLocation( const Location &location );

		Client &addClient( int fd );
		Client &getClient(int fd);
		void removeClients(int fd);
};

#endif