#include "Server.hpp"
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

int main(){

	std::string promt = "Welcome! \r\n";

	Server serv;

	//create a master socket
	serv.create_master_socket(AF_INET, SOCK_STREAM, 0);

	//type of socket created
	serv.set_address_socket("127.0.0.1", PORT, AF_INET);

	//bind the socket to localhost port
	serv.bind_master_socket();

	//try to specify maximum of 128 pending connections for the master socket
	listen_socket(serv.get_master_socket(), SOMAXCONN);
	
	fd_set	readfds;
	int		max_sd;

	while (true){
		//clear the socket set
		FD_ZERO(&readfds);
	
		// add master socket to set;
		FD_SET(serv.get_master_socket(), &readfds);
		max_sd = serv.get_master_socket();

		// add child sockets to set
		for (int i = 0; i < MAX_CLIENTS; i++){
			// get socket descriptor
			int sd = serv.get_client_sd(i); 
		
			//if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);
			
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
		
		//wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
		int ret = select(max_sd + 1, &readfds, NULL, NULL, NULL); // • Select should check read and write at the same time.
		if (ret < 0)
			std::cerr << "Select error" << std::endl; // continue ???
		
		//If something happened on the master socket, then its an incoming connection
		if (FD_ISSET(serv.get_master_socket(), &readfds)){
			int new_socket = serv.accept_master_socket();

			if (new_socket < 0) //???
				continue;
			
			std::cout << "New connection, socket fd is " << new_socket << std::endl;

			// send a message to client
			if (send(new_socket, "Hola", 4, 0) < 0)
				std::cerr << "Send failed" << std::endl;
			
			//add new socket to array of sockets
			if (serv.set_new_socket(new_socket) < 0)
				std::cerr << "Adding error" << std::endl; // ???
		}
		//else its some IO operation on some other socket (write, except)


		for (int i = 0; i < MAX_CLIENTS; i++){
			int		sd = serv.get_client_sd(i);
			int		ret;
			char	buffer[10];
			if (FD_ISSET(sd, &readfds)){
				// Check if it closing or incoming message
				if ((ret = read(sd, buffer,1024)) == 0){
					std::cout << "Host disconnected, sd " << sd << std::endl;
					close(sd);
					serv.set_client_socket(i, 0);
				}
				// just read message and do some   
				else{
					buffer[ret] = '\0';
					std::cout << buffer << std::endl;

				}
			}
		}
	}
	return 0;
}