#include "Location.hpp"

Location::Location(): _methods(""),  _root(""), _cgi_extensions(""), _uploads_path(""), _redirect(""), _autoindex(false) {}

Location::~Location() {}

Location::Location(const Location& other): _methods(other._methods),  _root(other._root), _cgi_extensions(other._cgi_extensions), _uploads_path(other._uploads_path), _redirect(other._redirect), _autoindex(other._autoindex) {}

Location &Location::operator=(const Location& other) {
	if (this != &other)
	{
		_methods = other._methods;
		_root = other._root;
		_cgi_extensions = other._cgi_extensions;
		_uploads_path = other._uploads_path;
		_redirect = other._redirect;
		_autoindex = other._autoindex;
	}
	return *this;
}
