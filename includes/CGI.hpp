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
#include "Request.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "RequestConfigMatch.hpp"


#define STDIN 0
#define STDOUT 1

class CGI {
private:
	char								**env;
	char								**argv;
	char								*dir;
	size_t								envCount;
	std::map<std::string, std::string>	envMap;
	std::string							PathInfo;
	int									fd[2];
	std::string							body;
	CGI(){};
public:
	CGI(Request &req, Server &ser);
	~CGI();

	void	init(Request &req, Server &ser);
	void	setAuthorization(Request &req);
	void	creatENV();
	void	exec(Request &req, Server &ser);
	void	mallocError(int count);

};


#endif
