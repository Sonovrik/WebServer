#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <vector>


typedef struct		s_location{
	std::string		_name;
	std::map<std::string, std::string>	_directives;
}					location_t;


	// socket funcs
void	allow_mul_cons_socket(int socket);
int		create_socket(int domain, int type, int protocol);
void	bind_socket(int socket, sockaddr_in *addr, socklen_t addrlen);
void	listen_socket(int master_socket, int max_connections);
std::string		ipToString(uint32_t addr);


bool location_tCompare(const location_t &x, const location_t &y);

// errors
std::string	get_errorPage(int code);


// MIME types
void			initMimeTypes(void);
std::string		findMimeType(const std::string &ext);


#endif