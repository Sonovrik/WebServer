#include "Server.hpp"

//////////////////////////   Coplin   \\\\\\\\\\\\\\\\\\\\\\\\\\

Server::Server():
	_masterSocket(0),
	_addrlen(0),
	_serverName(""),
	_ip(""),
	_port(""),
	_root(""),
	_maxBodySize(""),
	_maxBufferSize(""),
	_maxSd(0){}

Server::~Server(){
	if (!SERVER_CLOSE)
		return ;
	std::vector<Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end()){
		close(it->getSd());
		it++;
	}
	close (this->_masterSocket);
}

Server	&Server::operator=(Server const &other){
	if (this != &other){
		this->_masterSocket = other._masterSocket;
		this->_addrlen = other._addrlen;
		this->_addr = other._addr;
		this->_maxSd = other._maxSd;
		this->_env = other._env;

		this->_clients = other._clients;

		this->_maxBufferSize = other._maxBufferSize;
		this->_serverName = other._serverName;
		this->_ip = other._ip;
		this->_port = other._port;
		this->_root = other._root;
		this->_maxBodySize = other._maxBodySize;
		this->_errorPages = other._errorPages;
		this->_locations = other._locations;
	}
	return *this;
}

Server::Server(Server const &other){
	*this = other;
}

//////////////////////////   Coplin   \\\\\\\\\\\\\\\\\\\\\\\\\\




//////////////////////////   Full Envs   \\\\\\\\\\\\\\\\\\\\\\\\\\

void		Server::sortLocations(void){
	std::vector<location_t>::iterator	it =this->_locations.begin();

	for (; it !=this->_locations.end(); it++){
		if (it->_name.front() != '/')
			it->_name.insert(it->_name.begin(), '/');
		if (it->_name.back() != '/')
			it->_name.push_back('/');
	}
	std::sort(_locations.begin(),this->_locations.end(), location_tCompare);
}

void		Server::fullConfigEnvironment(void){
	this->_env.insert(std::make_pair<std::string, std::string>("AUTH_TYPE", "Basic"));
	this->_env.insert(std::make_pair<std::string, std::string>("GATEWAY_INTERFACE", "CGI/1.1"));
	this->_env.insert(std::make_pair<std::string, std::string>("SERVER_NAME", this->_serverName));
	this->_env.insert(std::make_pair<std::string, std::string>("SERVER_PORT", this->_port));
	this->_env.insert(std::make_pair<std::string, std::string>("SERVER_PROTOCOL", "HTTP/1.1"));
	this->_env.insert(std::make_pair<std::string, std::string>("SERVER_SOFTWARE", "TOXIGEN/1.0"));
}

void		Server::fullBasicDirectives(void){
	if (this->_serverName.empty())
		this->_serverName =this->_ip + ":" +this->_port;
	if (this->_root.empty())
		this->_root = ".";
	if (this->_maxBodySize.empty())
		this->_maxBodySize = "100000000";

	std::vector<location_t>::iterator it =this->_locations.begin();
	for (; it <this->_locations.end(); it++){
		if (it->_directives.find("root") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("root", this->_root + '/'));
		else{
			std::string tmp(this->_root + '/' + (*(it->_directives.find("root"))).second);
			it->_directives.erase("root");
			it->_directives.insert(std::make_pair<std::string, std::string>("root", tmp));
		}
		if (it->_directives.find("max_body_size") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("max_body_size",this->_maxBodySize));
		if (it->_directives.find("method") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("method", "GET HEAD PUT POST"));
		if (it->_directives.find("autoindex") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("autoindex", "off"));
		if (it->_directives.find("cgi_extensions") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("cgi_extensions", ".bla .php .py"));
		if (it->_directives.find("cgi_path") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("cgi_path", ""));
		if (it->_directives.find("upload_storage") == it->_directives.end())
			it->_directives.insert(std::make_pair<std::string, std::string>("upload_storage", "/storage"));
	}
}

void	Server::add_errorPage(const std::pair<int, std::string> &page){
	this->_errorPages.insert(page);
}

//////////////////////////   Full Envs   \\\\\\\\\\\\\\\\\\\\\\\\\\




//////////////////////////   For clients   \\\\\\\\\\\\\\\\\\\\\\\\\\

size_t		Server::delete_client(size_t index){
	if (index >= this->_clients.size())
		return -1;
	this->_clients.erase(this->_clients.begin() + index - 1);
	return this->_clients.size();
}

size_t		Server::delete_client(Client &client){
	this->_clients.begin();
	std::vector<Client>::iterator it = this->_clients.begin();
	for (; it != _clients.end(); it++){
		if (*it == client){
			this->_clients.erase(it);
			return this->_clients.size();
		}
	}
	throw OutOfRangeException();
}

void		Server::add_client(int newSd){
	this->_clients.push_back(Client(newSd));
}

//////////////////////////   For clients   \\\\\\\\\\\\\\\\\\\\\\\\\\




//////////////////////////   Sockets   \\\\\\\\\\\\\\\\\\\\\\\\\\


void	Server::addNewConnection(void){
	int new_connection = accept_masterSocket();
	fcntl(new_connection, F_SETFL, O_NONBLOCK);
	add_client(new_connection);
}

void	Server::create_masterSocket(int domain, int type, int protocol){
	int fd = 1;
	if((this->_masterSocket = socket(domain, type, protocol)) == 0)
		throw SocketCreateException();
	if(setsockopt(this->_masterSocket, SOL_SOCKET, SO_REUSEADDR, &fd, sizeof(int)) < 0)
		throw SocketCreateException();
}

void	Server::listen_socket(int max_connections){
	if (listen(this->_masterSocket, max_connections) < 0)
		throw SocketListenException();
}

void	Server::set_addressSocket(const char *ip, int port, int domain){
	this->_addr.sin_addr.s_addr = inet_addr(ip);
	this->_addr.sin_port = htons(port);
	this->_addr.sin_family = domain;
	this->_addrlen = sizeof(_addr);
}

void	Server::bind_masterSocket(void){
	if (bind(this->_masterSocket, (struct sockaddr *)&(this->_addr), this->_addrlen) < 0)
		throw SocketBindException();
}

int		Server::accept_masterSocket(void){
	int new_socket;
	sockaddr_in addr;
	if ((new_socket = accept(this->_masterSocket, (struct sockaddr *)&addr, (socklen_t*)&_addrlen)) < 0)
		throw SocketAcceptException();
	this->_env.insert(make_pair("REMOTE_ADDR", ipToString(addr.sin_addr.s_addr)));
	return (new_socket);
}

void		Server::FD_reset(fd_set *to_set){
	std::vector<Client>::const_iterator it = this->_clients.begin();
	this->_maxSd = this->_masterSocket;

	for (; it != this->_clients.end(); it++){
		int sd = it->getSd();
		if (sd > 0)
			FD_SET(sd, to_set);
		if (this->_maxSd < sd)
			this->_maxSd = sd;
	}
}

//////////////////////////   Sockets   \\\\\\\\\\\\\\\\\\\\\\\\\\



//////////////////////////   Setters   \\\\\\\\\\\\\\\\\\\\\\\\\\

void	Server::set_serverName(const std::string &name){
	this->_serverName = name;
}

void	Server::set_ip(const std::string& ip){
	this->_ip = ip;
}

void	Server::set_port(const std::string& port){
	this->_port = port;
}

void	Server::set_root(const std::string& root){
	this->_root = root;
}

void	Server::set_maxBodySize(const std::string& len){
	this->_maxBodySize = len;
}

void	Server::set_locations(std::vector<location_t>& locations){
	this->_locations = locations;
}

void	Server::set_master_socket(const int socket){
	this->_masterSocket = socket;
}

void	Server::set_addrlen(const socklen_t& len){
	this->_addrlen = len;
}

void	Server::set_sockaddress(const sockaddr_in& addr){
	this->_addr = addr;
}

void	Server::set_maxBufferSize(const std::string	&size){
	this->_maxBufferSize = size;
}

//////////////////////////   Setters   \\\\\\\\\\\\\\\\\\\\\\\\\\


//////////////////////////   Getters   \\\\\\\\\\\\\\\\\\\\\\\\\\

Client		&Server::get_Client(int index){
	if (index > this->_clients.size())
		throw std::exception();
	else
		return this->_clients[index - 1];
}

std::string const	&Server::getEnvValue(std::string const	&key) const{
	std::map<std::string, std::string>::const_iterator it = this->_env.find(key);
	if (it == this->_env.end())
		throw OutOfRangeException();
	else
		return it->second;
}

std::string const	Server::get_errorPath(int code) const{
	if (code < 0)
		return "";
	std::map<int, std::string>::const_iterator it = this->_errorPages.begin();
	if ((it = this->_errorPages.find(code)) != this->_errorPages.end())
		return it->second;
	return "";
}

std::string const	&Server::getLocationMethods(int pos) const{
	return this->_locations[pos]._directives.find("method")->second;
}

std::string const			&Server::get_port(void) const{
	return this->_port;
}

std::string const	&Server::get_root(void) const{
	return this->_root;
}

std::string const		&Server::get_maxBodySize(void) const{
	return this->_maxBodySize;
}

std::string const	&Server::get_serverName(void) const{
	return this->_serverName;
}

std::string const	&Server::get_ip(void) const{
	return this->_ip;
}

int			Server::get_master_socket(void) const{
	return this->_masterSocket;
}

socklen_t	Server::get_addrlen(void) const{
	return this->_addrlen;
}

sockaddr_in const	&Server::get_sockaddress(void) const{
	return this->_addr;
}

std::vector<location_t> const &Server::get_locations(void) const{
	return this->_locations;
}

size_t				Server::get_clientCount(void) const{
	return this->_clients.size();
}

int					Server::get_clientsd(size_t index) const{
	if (index > this->_clients.size())
		return -1;
	else
		return this->_clients[index - 1].getSd();
}

int			Server::get_maxSd(void) const{
	return this->_maxSd;
}

std::string const	&Server::get_maxBufferSize(void) const{
	return this->_maxBufferSize;
}

std::map<int, std::string>	&Server::get_ErrorPaths(void){
	return this->_errorPages;
}


//////////////////////////   Getters   \\\\\\\\\\\\\\\\\\\\\\\\\\



//////////////////////////   Exceptions   \\\\\\\\\\\\\\\\\\\\\\\\\\

const char *Server::SocketCreateException::what() const throw(){
	return "\033[31mSocket creation failed\033[0m";
}

const char *Server::SocketAcceptException::what() const throw(){
	return "\033[31mSocket accept failed\033[0m";
}

const char *Server::SocketBindException::what() const throw(){
	return "\033[31mSocket bind failed\033[0m";
}

const char *Server::OutOfRangeException::what() const throw(){
	return "\033[31mOut of range\033[0m";
}

const char *Server::SocketListenException::what() const throw(){
	return "\033[31mSocket listening failed\033[0m";
}

//////////////////////////   Exceptions   \\\\\\\\\\\\\\\\\\\\\\\\\\


