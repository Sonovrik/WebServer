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
#include <fcntl.h>
#include <sys/time.h>
#include "utils.hpp"
#include "Client.hpp"

static bool	SERVER_CLOSE;

class Server{

private:

	// socket varibles
	int			_masterSocket;
	socklen_t	_addrlen;
	sockaddr_in	_addr;
	int			_maxSd;
	std::map<std::string, std::string>	_env;

	// clients
	std::vector<Client>		_clients;

	// config varibles
	std::string		_serverName;
	std::string		_ip;
	std::string		_port;
	std::string		_root;
	std::string		_maxBufferSize;
	std::string		_maxBodySize;
	std::map<int, std::string>	_errorPages;
	std::vector<location_t>	_locations;

public:
	// Coplin
	Server();
	~Server();
	Server(Server const &);
	Server &operator=(Server const &);

	// methods for Clients
	void		add_client(int newSd);
	size_t		get_clientCount(void) const;
	int			get_clientsd(size_t index) const;
	Client		&get_Client(int index);
	size_t		delete_client(size_t index);
	size_t		delete_client(Client &client);

	// full variables methods
	void		fullConfigEnvironment(void);
	void		fullBasicDirectives(void);
	void		sortLocations(void);

	// socket realize methods
	void	create_masterSocket(int, int, int);
	void	set_addressSocket(const char *, int, int);
	void	bind_masterSocket();
	int		accept_masterSocket();
	void	listen_socket(int max_connections);
	void	addNewConnection(void);

	// setters
	void	set_serverName(const std::string	&name);
	void	set_root(const std::string	&root);
	void	set_maxBodySize(const std::string	&size);
	void	add_errorPage(const std::pair<int, std::string> &page);
	void	set_locations(std::vector<location_t>	&locaton);
	void	set_port(const std::string	&port);
	void	set_ip(const std::string	&ip);
	void	set_maxBufferSize(const std::string	&size);

	// setters for sockets
	void	set_master_socket(const int socket);
	void	set_addrlen(const socklen_t& len);
	void	set_sockaddress(const sockaddr_in& addr);
	void	FD_reset(fd_set *);
	void	add_sd(int sd);

	// getters
	std::string const	&get_serverName(void) const;
	std::string const	&get_ip(void) const;
	std::string const	&get_port(void) const;
	std::string const	&get_root(void) const;
	std::string const	&get_maxBodySize(void) const;
	std::string const	get_errorPath(int code) const;
	std::vector<location_t> const	&get_locations(void) const;
	std::string const	&getLocationMethods(int pos) const;
	std::string const	&get_maxBufferSize(void) const;
	std::map<int, std::string>	&get_ErrorPaths(void);


	// getters for sockets
	int			get_maxSd(void) const;
	int			get_master_socket(void) const;
	socklen_t	get_addrlen(void) const;
	sockaddr_in const	&get_sockaddress(void) const;

	std::string const	&getEnvValue(std::string const &key) const;


	class SocketCreateException : public std::exception{
		const char *what() const throw();
	};

	class SocketBindException: public std::exception{
		const char *what() const throw();
	};

	class OutOfRangeException: public std::exception{
		const char *what() const throw();
	};

	class SocketListenException: public std::exception{
		const char *what() const throw();
	};

	class SocketAcceptException: public std::exception{
		const char *what() const throw();
	};
};

#endif