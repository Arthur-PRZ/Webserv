#include <iostream>

class Location {
	private:
		const std::string methods;
		const std::string root;
		const std::string cgi_extensions;
		const std::string uploads_path;
		const std::string redirect;
		bool autoindex;

	public:
		Location();
		~Location();
		Location(const Location& other);
		Location &operator=(const Location& other);
};