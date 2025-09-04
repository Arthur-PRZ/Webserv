#ifndef _LOCATION_HPP_ 
#define _LOCATION_HPP_ 
#include <string>

class Location {
	private:
		std::string _methods;
		std::string _root;
	 	std::string _cgiExtensions;
	 	std::string _uploadsPath;
		std::string _redirect;
		bool _autoindex;

	public:
		Location();
		~Location();
		Location(const Location& other);
		Location &operator=(const Location& other);

		const std::string &getMethods( void ) const;
		const std::string &getRoot( void ) const;
		const std::string &getCGIExtensions( void ) const;
		const std::string &getUploadsPath( void ) const;
		const std::string &getRedirect( void ) const;
		bool getAutoIndex( void ) const;

		void setMethods( const std::string& methods );
		void setRoot( const std::string& root );
		void setCGIExtensions( const std::string& cgiExtensions );
		void setUploadsPath( const std::string& uploadsPath );
		void setRedirect( const std::string& redirect);
		void setAutoIndex( const bool autoindex );
};

#endif