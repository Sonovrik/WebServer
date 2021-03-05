#include "utils.hpp"

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

std::string		ipToString(uint32_t addr) {
	std::string ret;
	ret.append(std::to_string(addr & 0xff));
	ret.append(".");
	ret.append(std::to_string((addr >> 8) & 0xff));
	ret.append(".");
	ret.append(std::to_string((addr >> 16) & 0xff));
	ret.append(".");
	ret.append(std::to_string(addr >> 24));
	return ret;
}