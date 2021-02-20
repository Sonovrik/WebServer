#include "Server.hpp"

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





Server::Server(): 			// ???
	_master_socket(0),
	_addrlen(0){
	for (int i = 0; i < MAX_CLIENTS; i++)
		_clients_sockets[i] = 0;
	
	// std::pair<std::string, std::string> pairs[17];
	// for (int i = 0; i < MAX_HEADERS; i++){
	// 	pairs[i].first = this->_env_names[i];
	// }
	// _env_names = ;
	// for (int i = 0; i < 17; i++){
	// 	pairs[i].first = 
	// }
}

Server::~Server(){}

Server	&Server::operator=(Server const &other){
	if (this != &other){
		this->_master_socket = other._master_socket;
		this->_addrlen = other._addrlen;
		this->_addr = other._addr;
		this->_env = other._env;
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
	_addr.sin_port = htons(port);
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
