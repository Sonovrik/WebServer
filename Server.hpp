#ifndef SERVER_HPP
#define SERVER_HPP


#include <map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8000
#define MAX_CLIENTS 30 // ???

class Server{

private:
	// socket part
	int			_master_socket;
	int			_clients_sockets[MAX_CLIENTS];
	socklen_t	_addrlen;
	sockaddr_in	_addr;
	
	// parse part (request)
	std::string		_method;
	std::string		_path;
	std::string		_version;
	std::map<std::string,std::string>	_headers;
	std::string		_body;
	
	// response ???

	// parse env
	std::map<std::string, std::string>	_env;

public:
	Server();
	~Server();
	Server &operator=(Server const &);

	// socket methods подумать как дописать.
	void	allow_mul_cons_socket(int socket);
	int		create_socket(int domain, int type, int protocol);
	void	bind_socket(int socket, sockaddr_in *addr, socklen_t addrlen);
	void	listen_socket(int master_socket, int max_connections);

	// setters
	void	set_master_socket(int);
	void	set__addrlen(socklen_t);
	void	set_sockaddress(sockaddr_in);

	void	set_method(std::string);
	void	set_path(std::string);
	void	set_version(std::string);
	void	set_body(std::string);

	void	set_headers(std::map<std::string,std::string>);


	// getters
	int			get_master_socket(void) const;
	socklen_t	get__addrlen(void) const;
	sockaddr_in const	&get_sockaddress(void) const;

	std::string const	&get_method(void) const;
	std::string	const	&get_path(void) const;
	std::string	const	&get_version(void) const;
	std::string	const	&get_body(void) const;

	std::map<std::string,std::string> const	get_headers(void);
};


#endif