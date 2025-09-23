#include "Image.hpp"
#include "RequestManagement.hpp"
#include <sys/types.h>

Image::Image() : _content(""), _filename("") {}

Image::~Image() {}

Image::Image(const Image& other) : _content(other._content), _filename(other._filename) {}

Image &Image::operator=(const Image& other) {
	if (this != &other) {
		_content = other._content;
		_filename = other._filename;
	}
	return *this;
}

void Image::parseImage(std::string &boundary, std::string &body) {
	setFileName(boundary, body);
	setFileContent(boundary, body);
}

void Image::setFileName(std::string &boundary, std::string &body) {
	ssize_t pos;
	ssize_t posBefore;
	ssize_t filenamePos;

	pos = body.find(boundary);
	pos = body.find("filename", pos);
	pos += 10;
	posBefore = pos;
	filenamePos = body.find("\"", pos);
	_filename = body.substr(posBefore, filenamePos - pos);
}

void Image::setFileContent(std::string &boundary, std::string &body) {
	ssize_t pos;
	ssize_t posBefore;
	ssize_t contentPos;

	pos = body.find(boundary);
	pos = body.find("\r\n\r\n", pos);
	pos += 4;
	posBefore = pos;
	contentPos = body.find(boundary, pos);
	_content = body.substr(posBefore, contentPos - pos - 2);
	std::cout << "CONTENT IS ::::" << _content << std::endl;	
}