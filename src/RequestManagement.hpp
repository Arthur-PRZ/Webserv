#ifndef _REQUESTMANAGEMENT_HPP_
#define _REQUESTMANAGEMENT_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include "Server.hpp"
#include "Image.hpp"

class RequestManagement
{
    private :
	    std::string _method;
	    std::string _path;
	    std::string _httpVer;
	    std::string _body;
	    std::string _extensionType;
		std::string _contentType;
	    bool _methodFound;
	    bool _pageFound;
	    bool _goodVer;
		Server _server;
		Image _image;

    public :
		RequestManagement();
	    RequestManagement(Server server);
	    ~RequestManagement();
	    RequestManagement(const RequestManagement &copy);
	    RequestManagement &operator=(const RequestManagement &other);
	    
	    void parser(std::string &request, int &client_fd);
	    void setBool(std::string &request, int &client_fd);
	    void setBody(std::string &request, int &client_fd);
	    void setExtensionType();
	    void setContentType(std::string &request);
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
};

#endif