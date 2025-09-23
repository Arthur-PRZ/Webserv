#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <string>

class Image {
	private:
		std::string _content;
		std::string _filename;

	public:
		Image();
		~Image();
		Image(const Image& other);		
		Image &operator=(const Image& other);

		void parseImage(std::string &boundary, std::string &body);
		void setFileName(std::string &boundary, std::string &body);
		void setFileContent(std::string &boundary, std::string &body);
		std::string &getContent();
		std::string &getFilename();
};

#endif