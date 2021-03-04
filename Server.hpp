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

typedef struct		s_location{
	std::string		_name;
	std::map<std::string, std::string>	_directives;
}					location_t;

bool location_tCompare(const location_t &x, const location_t &y);

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


	// config part
	std::string		_serverName;
	std::string		_ip;
	int				_port;
	std::string		_root;
	int				_maxBodySize;
	std::string		_errorPage;
	std::vector<location_t>	_locations;




public:
	Server();
	~Server();
	Server(Server const &);
	Server &operator=(Server const &);


	void		fullBasicDirectives(void);
	void		sortLocations(void);

	// socket realize part
	void	create_master_socket(int, int, int);
	void	set_address_socket(const char *, int, int);
	void	bind_master_socket();
	int		accept_master_socket();

	// setters
	void	set_serverName(std::string);
	void	set_ipToListen(std::string);
	void	set_root(std::string);
	void	set_maxBodySize(int);
	void	set_errorPage(std::string);
	void	set_locations(std::vector<location_t>);
	void	set_port(int);
	void	set_ip(std::string);

	// setters for sockets
	void	set_master_socket(int);
	void	set_addrlen(socklen_t);
	void	set_sockaddress(sockaddr_in);
	int		set_new_socket(int);
	void	set_client_socket(int, int);


	// getters
	std::string	get_serverName(void);
	std::string	get_ip(void);
	int			get_port(void);
	std::string	get_root(void);
	int			get_maxBodySize(void);
	std::string	get_errorPage(void);
	std::vector<location_t>	get_locations(void);

	int			get_master_socket(void) const;
	int			get_client_sd(int) const;
	socklen_t	get_addrlen(void) const;
	sockaddr_in const	&get_sockaddress(void) const;
};


#endif