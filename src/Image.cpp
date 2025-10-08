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
	std::string::size_type pos;
	ssize_t posBefore;
	std::string::size_type filenamePos;

	pos = body.find(boundary);
	if (pos == std::string::npos)
		return;
	pos = body.find("filename", pos);
	if (pos == std::string::npos)
		return;
	pos += 10;
	posBefore = pos;
	filenamePos = body.find("\"", pos);
	if (filenamePos == std::string::npos)
		return;
	_filename = body.substr(posBefore, filenamePos - pos);
}

void Image::setFileContent(std::string &boundary, std::string &body) {
	std::string::size_type pos;
	ssize_t posBefore;
	std::string::size_type contentPos;
	std::cout << "body: " << body << std::endl;
	std::cout << "boundary: "<< boundary << std::endl;

	pos = body.find(boundary);
	if (pos == std::string::npos)
	{
		std::cout << "return 1" << std::endl;
		return;
	}
	pos = body.find("\r\n\r\n", pos);
	if (pos == std::string::npos)
	{
		std::cout << "return 2" << std::endl;
		return;
	}
	pos += 4;
	posBefore = pos;
	contentPos = body.find(boundary, pos);
	if (contentPos == std::string::npos)
	{
		std::cout << "return 3" << std::endl;
		return;
	}
	_content = body.substr(posBefore, contentPos - pos - 2);
	std::cout << "The content of the image is :" << _content << std::endl;
}

std::string &Image::getContent() {
	return _content;
}

std::string &Image::getFilename() {
	return _filename;
}