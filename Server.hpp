#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <array>
#include "utils.hpp"

class Server{

private:
	// socket part
	int			_master_socket;
	std::vector<int>	_clients_sockets;
	socklen_t	_addrlen;
	sockaddr_in	_addr;
	int			_maxSd;
	static std::array<std::string, 17> const _env_names; 
	std::map<std::string, std::string>	_env;


	// config part
	std::string		_serverName;
	std::string		_ip;
	std::string		_port;
	std::string		_root;
	std::string		_maxBodySize;
	std::string		_errorPage;
	std::vector<location_t>	_locations;

public:
	Server();
	~Server();
	Server(Server const &);
	Server &operator=(Server const &);


	void		fullConfigEnvironment(void); // full it
	void		fullBasicDirectives(void);
	void		sortLocations(void);

	// socket realize part
	void	create_master_socket(int, int, int);
	void	set_address_socket(const char *, int, int);
	void	bind_master_socket();
	int		accept_master_socket();

	// setters
	void	set_serverName(const std::string&);
	void	set_root(const std::string&);
	void	set_maxBodySize(const std::string&);
	void	set_errorPage(const std::string&);
	void	set_locations(std::vector<location_t>&);
	void	set_port(const std::string&);
	void	set_ip(const std::string&);

	// setters for sockets
	void	set_master_socket(const int socket);
	void	set_addrlen(const socklen_t& len);
	void	set_sockaddress(const sockaddr_in& addr);
	void	FD_reset(fd_set *);
	void	add_sd(int sd);

	// getters
	std::string			get_serverName(void) const;
	std::string			get_ip(void) const;
	std::string			get_port(void) const;
	std::string			get_root(void) const;
	std::string			get_maxBodySize(void) const;
	std::string			get_errorPage(void) const;
	std::vector<location_t>	get_locations(void) const;
	size_t				get_clientCount(void) const;
	int					get_clientsd(size_t index) const;

	int			get_maxSd(void) const;
	int			get_master_socket(void) const;
	socklen_t	get_addrlen(void) const;
	sockaddr_in const	&get_sockaddress(void) const;

	size_t		delete_client(size_t index);
};


template<class T>
bool			isInArray(T first, T last, const std::string &line){
	size_t pos = 0;
	for (; first != last; first++){
		if ((pos = line.find(*first)) != std::string::npos)
				return true;
	}
	return false;
}

#endif