#include "Server.hpp"
#include <algorithm>
const std::string Server::_env_names[] = {"AUTH_TYPE", "CONTENT_LENGTH",
		"CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO", "PATH_TRANSLATED", "QUERY_STRING",
		"REMOTE_ADDR", "REMOTE_IDENT", "REMOTE_USER", "REQUEST_METHOD", "REQUEST_URI",
		"SCRIPT_NAME", "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWARE"};

// AUTH_TYPE - Basic
// CONTENT_LENGTH - ???
// CONTENT_TYPE - text/html
// GATEWAY_INTERFACE - CGI/1.1
// PATH_INFO - /sports.html Например, если задан путь c:/cgi-bin/example1.exe/sports.html
// PATH_TRANSLATED - c:\sports.html Например, если переменная PATH_TRANSLATED имеет значение /sports.html, а корневым дирикторием сервера служит c:\,
// QUERY_STRING - name=margaret+alarcon Например, для URL http://www.jamsa.com/cgi-bin/grandma.exe?name=margaret+alarcon
// REMOTE_ADDR - 204.212.52.209 ( ip browser )
// REMOTE_IDENT - pschmauder.www.jamsa.com  Напрмер, если имя удаленного пользователя pschmauder и он назодится на удаленном узлеjamsa.com
// REMOTE_USER - pschmauder Например, предположим, что именем удаленного пользователя является pschmauder 
// REQUEST_METHOD - GET
// REQUEST_URI - URI
// SCRIPT_NAME - cgi-bin/example1.exe Например, если имеется URL http://www.jamsa.com/cgi-bin/someprog.exe
// SERVER_NAME - 204.212.52.209 (ip host)
// SERVER_PORT - 8000
// SERVER_PROTOCOL - HTTP/1.1
// SERVER_SOFTWARE - FolkWeb/1.01 Формат имени Web-сервера и номер версии должен передаваться CGI следующим образом: имя/версия.

bool location_tCompare(const location_t &x, const location_t &y){
	size_t	a = std::count(x._name.begin(), x._name.end(), '/');
	size_t	b = std::count(y._name.begin(), y._name.end(), '/');
	if (a > b)
		return true;
	if (a < b)
		return false;
	if (a == b){
		if (x._name > y._name)
			return true;
	}
	return false;
}

void		Server::sortLocations(void){
	std::string		temp("");
	std::vector<location_t>::iterator	it = _locations.begin();
	for (; it != _locations.end(); it++){
		if ((*it)._name.front() != '/')
			(*it)._name.insert((*it)._name.begin(), '/');
		if ((*it)._name.back() != '/')
			(*it)._name.push_back('/');
	}
	std::sort(_locations.begin(), _locations.end(), location_tCompare);
}

#include <sstream>

void		Server::fullBasicDirectives(void){
	// if (_serverName.empty())
		_serverName = _ip + ":" + static_cast<std::ostringstream*>( &(std::ostringstream() << _port) )->str();;
	std::cout << _serverName << std::endl;
}


Server::Server():
	_master_socket(0),
	_addrlen(0),
	_serverName(""),
	_ip(""),
	_port(0),
	_root(""),
	_maxBodySize(0),
	_errorPage(""){
	for (int i = 0; i < MAX_CLIENTS; i++)
		_clients_sockets[i] = 0;
}

Server::~Server(){}

Server	&Server::operator=(Server const &other){
	if (this != &other){
		this->_master_socket = other._master_socket;
		this->_addrlen = other._addrlen;
		this->_addr = other._addr;
		this->_env = other._env;
	
		this->_serverName = other._serverName;
		this->_ip = other._ip;
		this->_port = other._port;
		this->_root = other._root;
		this->_maxBodySize = other._maxBodySize;
		this->_errorPage = other._errorPage;
		this->_locations = other._locations;
	}
	return *this;
}

Server::Server(Server const &other){
	*this = other; // ???
}

void	Server::create_master_socket(int domain, int type, int protocol){
	_master_socket = create_socket(domain, type, protocol);
	allow_mul_cons_socket(_master_socket);
}

void	Server::set_address_socket(const char *ip, int port, int domain){
	_addr.sin_addr.s_addr = inet_addr(ip);
	_addr.sin_port = htons(port); //(port << 8) >> 8
	_addr.sin_family = domain;
	_addrlen = sizeof(_addr);
}

void	Server::bind_master_socket(){
	bind_socket(this->_master_socket, &(this->_addr), this->_addrlen);
}

int		Server::accept_master_socket(){
	int new_socket;
	if ((new_socket = accept(_master_socket, (struct sockaddr *)&_addr, (socklen_t*)&_addrlen)) < 0){
			std::cerr << "Accept failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (new_socket);
}

// setters

void	Server::set_serverName(std::string _name){
	this->_serverName = _name;
}

void	Server::set_ip(std::string ip){
	this->_ip = ip;
}

void	Server::set_port(int port){
	this->_port = port;
}

void	Server::set_root(std::string root){
	this->_root = root;
}

void	Server::set_maxBodySize(int len){
	this->_maxBodySize = len;
}

void	Server::set_errorPage(std::string page){
	this->_errorPage = page;
}

void	Server::set_locations(std::vector<location_t>	locations){
	this->_locations = locations;
}

int		Server::set_new_socket(int socket){
	for (int i = 0; i < MAX_CLIENTS; i++){
		if (this->_clients_sockets[i] == 0){
			this->_clients_sockets[i] = socket;
			std::cout << "Adding to list of sockets as " << i << std::endl;
			return 0;
		}
	}
	return -1;
}

void	Server::set_client_socket(int number, int sd){
	this->_clients_sockets[number] = sd;
}

void	Server::set_master_socket(int socket){
	this->_master_socket = socket;
}

void	Server::set_addrlen(socklen_t len){
	this->_addrlen = len;
}

void	Server::set_sockaddress(sockaddr_in addr){
	this->_addr = addr;
}


	// getters

int			Server::get_port(void){
	return _port;
}

std::string	Server::get_root(void){
	return _root;
}

int			Server::get_maxBodySize(void){
	return _maxBodySize;
}

std::string	Server::get_serverName(void){
	return _serverName;
}

std::string	Server::get_ip(void){
	return _ip;
}

std::string	Server::get_errorPage(void){
	return _errorPage;
}

int			Server::get_master_socket(void) const{
	return _master_socket;
}

socklen_t	Server::get_addrlen(void) const{
	return _addrlen;
}

sockaddr_in const	&Server::get_sockaddress(void) const{
	return _addr;
}

int					Server::get_client_sd(int number) const{
	return (_clients_sockets[number]);
}

// socket methods
void	allow_mul_cons_socket(int socket){
	int fd = 1;
	if( setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &fd, sizeof(int)) < 0 ){
		std::cerr << "Error: setsockopt" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int		create_socket(int domain, int type, int protocol){
	int new_socket;
	if((new_socket = socket(domain, type, protocol)) == 0){
		std::cerr << "Socket create failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	return new_socket;
}

void	bind_socket(int socket, sockaddr_in *addr, socklen_t addrlen){
	if (bind(socket, (struct sockaddr *)addr, addrlen)<0){
		std::cerr << "Bind failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	listen_socket(int master_socket, int max_connections){
	if (listen(master_socket, max_connections) < 0){
		std::cerr << "Listen failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}
