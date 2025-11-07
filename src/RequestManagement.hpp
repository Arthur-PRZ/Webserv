#ifndef _REQUESTMANAGEMENT_HPP_
#define _REQUESTMANAGEMENT_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include "Server.hpp"
#include "Image.hpp"
#include <poll.h>

class RequestManagement
{
    private :
	    std::string _method;
	    std::string _path;
	    std::string _httpVer;
	    std::string _body;
	    std::string _extensionType;
		std::string _contentType;
		std::string _urlPath;
		std::string _fileToDelete;
	    bool _methodFound;
	    bool _pageFound;
	    bool _goodVer;
		bool _authorizedMethod;
		Server _server;
		Image _image;

    public :
		RequestManagement();
	    RequestManagement(Server server);
	    ~RequestManagement();
	    RequestManagement(const RequestManagement &copy);
	    RequestManagement &operator=(const RequestManagement &other);
	    
	    void parser(std::string &request);
	    void setBool();
	    void setExtensionType();
	    void setContentType(std::string &request);
		void setClientBody(std::string body);
	    bool checkPath();

	    static int toInt(std::string &str);
		static unsigned long toUnsignedLong(std::string &str);

	    std::string &getMethod();
	    std::string &getPath();
	    std::string &getHttpVer();
	    std::string &getBody();
		Image getImage();
		std::string &getExtensionType();
		bool getMethodFound();
	    bool getPageFound();
	    bool getGoodVer();
		bool isMethodAuthorized();
		std::string &getUrlPath();
		std::string &getFileToDelete();

		Location* findMatchingLocation(const std::string &urlPath);
		std::string buildPhysicalPath(const std::string &urlPath, Location *location);
};

#endif