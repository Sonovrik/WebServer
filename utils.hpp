#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <vector>

class Server;


// struct for locations
typedef struct		s_location{
	std::string		_name;
	std::map<std::string, std::string>	_directives;
}					location_t;


// container search
template<class T>
bool			isInArray(T first, T last, const std::string &line){
	size_t pos = 0;
	for (; first != last; first++){
		if ((pos = line.find(*first)) != std::string::npos)
				return true;
	}
	return false;
}

void			trimString(std::string &line);
void			cleanString(std::string &str);

std::string	const	ipToString(uint32_t addr);

bool location_tCompare(const location_t &x, const location_t &y);

std::string		to_string(long long number);

// errors
std::string	get_errorPage(int code);


// MIME types
void			initMimeTypes(void);
std::string	const	findMimeType(const std::string &ext);



#include "Server.hpp"
#endif