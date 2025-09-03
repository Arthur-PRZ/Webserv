#include "Location.hpp"

Location::Location(): methods("Unknown"),  root("Unknown"), cgi_extensions("Unknown"), uploads_path("Unknown"), redirect("Unknown"), autoindex(false) {}

Location::~Location() {}

Location::Location(const Location& other): methods(other.methods),  root(other.root), cgi_extensions(other.cgi_extensions), uploads_path(other.uploads_path), redirect(other.redirect), autoindex(other.autoindex) {}

Location &Location::operator=(const Location& other) {
	if (this != &other)
	{}
	return *this;
}
