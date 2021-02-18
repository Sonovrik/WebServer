#include "Server.hpp"

Server::Server(): 			// ???
	_master_socket(0),
	_addrlen(0),
	_method(""),
	_path(""),
	_version(""),
	_body(""){
		for (int i = 0; i < MAX_CLIENTS; i++)
			_clients_sockets[i] = 0;
}

Server::~Server(){}

Server	&Server::operator=(Server const &other){
	if (this != &other){
		this->_master_socket = other._master_socket;
		// this->_clients_sockets = other._clients_sockets; ???
		this->_addrlen = other._addrlen;
		this->_addr = other._addr;
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_env = other._env;
	}
}

void	Server::set_master_socket(int socket){
	this->_master_socket = socket;
}

void	Server::set__addrlen(socklen_t len){
	this->_addrlen = len;
}

void	Server::set_sockaddress(sockaddr_in addr){
	this->_addr = addr;
}

void	Server::set_method(std::string method){
	this->_method = method;
}

void	Server::set_path(std::string path){
	this->_path = path;
}

void	Server::set_version(std::string version){
	this->_version = version;
}

void	Server::set_body(std::string body){
	this->_body = body;
}

void	Server::set_headers(std::map<std::string,std::string> map){
	this->_headers = map;
}


	// getters
int			Server::get_master_socket(void) const{
	return _master_socket;
}

socklen_t	Server::get__addrlen(void) const{
	return _addrlen;
}

sockaddr_in const	&Server::get_sockaddress(void) const{
	return _addr;
}


std::string const	&Server::get_method(void) const{
	return _method;
}

std::string const	&Server::get_path(void) const{
	return _path;
}

std::string const	&Server::get_version(void) const{
	return _version;
}

std::string const	&Server::get_body(void) const{
	return _body;
}

std::map<std::string,std::string> const	Server::get_headers(void){
	return this->_headers;
}

// socket methods
void	Server::allow_mul_cons_socket(int socket){
	int fd = 1;
	if( setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &fd, sizeof(int)) < 0 ){
		std::cerr << "Error: setsockopt" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int		Server::create_socket(int domain, int type, int protocol){
	int new_socket;
	if((new_socket = socket(domain, type, protocol)) == 0){
		std::cerr << "Socket create failed" << std::endl;
		exit(EXIT_FAILURE); // ???
	}
	return new_socket;
}

void	Server::bind_socket(int socket, sockaddr_in *addr, socklen_t addrlen){
	if (bind(socket, (struct sockaddr *)addr, addrlen)<0){
		std::cerr << "Bind failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Server::listen_socket(int master_socket, int max_connections){
	if (listen(master_socket, max_connections) < 0){
		std::cerr << "Listen failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}
