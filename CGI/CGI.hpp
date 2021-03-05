//
// Created by Kory Moaning on 2/26/21.
//

#ifndef CGI_HPP
#define CGI_HPP

#include <map>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include "../parser/Request.hpp"
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
	int fd[2];
	CGI(){};
public:
	CGI(Request &req, Server &ser);
	~CGI();
	CGI(const CGI &copy);
	CGI& operator= (CGI const &copy);
	void init(Request &req, Server &ser);
	void creatENV();
	void exec();
	//на выходе после выполнения скрипта формируется боди и возможно часть хедеров для RESPONSE
};


#endif
