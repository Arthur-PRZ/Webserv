#include <iostream>

class Location {
	private:
		std::string _methods;
		std::string _root;
	 	std::string _cgi_extensions;
	 	std::string _uploads_path;
		std::string _redirect;
		bool _autoindex;

	public:
		Location();
		~Location();
		Location(const Location& other);
		Location &operator=(const Location& other);
};