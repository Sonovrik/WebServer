//
// Created by Kory Moaning on 2/26/21.
//

#ifndef CGI_HPP
#define CGI_HPP

#include <map>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include "../Request/Request.hpp"
#include "../Server.hpp"
#include "../ConfigParser/ConfigParser.hpp"


#define STDIN 0
#define STDOUT 1

class CGI {
private:
	char **env;
	char **argv;
	int envCount;
	std::string RequestBody;
	std::string ResponseBody;
	std::map<std::string, std::string> envMap;
	std::string PathInfo;
	int fd[2];
	CGI(){};
public:
	CGI(Request &req, Server &ser);
	~CGI();
	CGI(const CGI &copy);
	CGI& operator= (CGI const &copy);
	void init(Request &req, Server &ser);
	void creatENV();

	const std::string &getResponseBody() const;

	void setResponseBody(const std::string &responseBody);

	void exec();
};


#endif
