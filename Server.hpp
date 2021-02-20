#ifndef SERVER_HPP
#define SERVER_HPP


#include <map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#define PORT 8000
#define MAX_CLIENTS 30 // ???
#define MAX_HEADERS 17
	// socket funcs
void	allow_mul_cons_socket(int socket);
int		create_socket(int domain, int type, int protocol);
void	bind_socket(int socket, sockaddr_in *addr, socklen_t addrlen);
void	listen_socket(int master_socket, int max_connections);

class Server{

private:
	// socket part
	int			_master_socket;
	int			_clients_sockets[MAX_CLIENTS];
	socklen_t	_addrlen;
	sockaddr_in	_addr;
	
	// response ???

	// parse env
	// std::vector<std::string> const _env_names;
	static std::string const _env_names[]; 
	std::map<std::string, std::string>	_env;

public:
	Server();
	~Server();
	Server(Server const &);
	Server &operator=(Server const &);


	// socket realize part
	void	create_master_socket(int, int, int);
	void	set_address_socket(const char *, int, int);
	void	bind_master_socket();
	int		accept_master_socket();

	// setters
	void	set_master_socket(int);
	void	set_addrlen(socklen_t);
	void	set_sockaddress(sockaddr_in);
	int		set_new_socket(int);
	void	set_client_socket(int, int);


	// getters
	int			get_master_socket(void) const;
	int			get_client_sd(int) const;
	socklen_t	get_addrlen(void) const;
	sockaddr_in const	&get_sockaddress(void) const;
};


#endif