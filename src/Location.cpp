#include "Location.hpp"
#include "iostream"

Location::Location()
	: _methods(""),
	  _root(""),
	  _cgiExtensions(""),
	  _uploadsPath(""),
	  _redirect(""),
	  _autoindex(false)
	   {}

Location::~Location() {}

Location::Location(const Location& other)
	: _methods(other._methods),
	  _root(other._root),
	  _cgiExtensions(other._cgiExtensions),
	  _uploadsPath(other._uploadsPath),
	  _redirect(other._redirect),
	  _autoindex(other._autoindex) {}

Location &Location::operator=(const Location& other) {
	if (this != &other)
	{
		_methods = other._methods;
		_root = other._root;
		_cgiExtensions = other._cgiExtensions;
		_uploadsPath = other._uploadsPath;
		_redirect = other._redirect;
		_autoindex = other._autoindex;
	}
	return *this;
}

const std::string &Location::getMethods( void ) const {
	return _methods;
}

const std::string &Location::getRoot( void ) const {
	return _root;
}


const std::string &Location::getCGIExtensions( void ) const {
	return _cgiExtensions;
}


const std::string &Location::getUploadsPath( void ) const {
	return _uploadsPath;
}


const std::string &Location::getRedirect( void ) const {
	return _redirect;
}


bool Location::getAutoIndex( void ) const {
	return _autoindex;
}

void Location::setMethods( const std::string& methods) {
	_methods = methods;
}

void Location::setRoot( const std::string& root) {
	_root = root;
}

void Location::setCGIExtensions( const std::string& cgiExtensions) {
	_cgiExtensions = cgiExtensions;
}

void Location::setUploadsPath( const std::string& uploadsPath) {
	_uploadsPath = uploadsPath;
}

void Location::setRedirect( const std::string& redirect) {
	_redirect = redirect;
}

void Location::setAutoIndex( const bool autoindex) {
	_autoindex = autoindex;
}

void Location::aff()
{
	// std::cout << _methods << std::endl;
}