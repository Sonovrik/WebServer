#include "Server.hpp"
#include <arpa/inet.h>
#include <sys/select.h>

int main(){

	std::string promt = "Welcome! \r\n";

	Server serv;

	//create a master socket
	serv.set_master_socket(create_socket(AF_INET, SOCK_STREAM, 0));
	
	//set master socket to allow multiple connections
	allow_mul_cons_socket(serv.get_master_socket());

	//type of socket created
	sockaddr_in	addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(PORT);
	addr.sin_family = AF_INET;

	serv.set__addrlen(sizeof(addr));
	serv.set_sockaddress(addr);
	//bind the socket to localhost port
	bind_socket(serv.get_master_socket, &addr, addrlen);
	return 0;
}