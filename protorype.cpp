#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>	//FD_SET, FD_ISSET, FD_ZERO macros
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // init_addr
#include <sys/select.h>

#define PORT 8000
#define MAX_CLIENTS 30

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
		std::cerr << "Socket failed" << std::endl;
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

int		main(void){
	int			master_socket;
	int			client_socket[MAX_CLIENTS] = {0};
	socklen_t	addrlen;
	sockaddr_in	addr;
	 
	char buffer[1025];  //data buffer of 1K

	//a message
	std::string message = "Welcome! \r\n";

	//create a master socket
	master_socket = create_socket(AF_INET, SOCK_STREAM, 0);

	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	allow_mul_cons_socket(master_socket);
 
	//type of socket created
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(PORT);
	addr.sin_family = AF_INET;
	addrlen = sizeof(addr);

	//bind the socket to localhost port
	bind_socket(master_socket, &addr, addrlen);

	//try to specify maximum of 128 pending connections for the master socket
	listen_socket(master_socket, SOMAXCONN);

	//set of socket descriptors
	fd_set	readfds;
	int		max_sd;
	int		sd;
	while(true){
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		
		//add child sockets to set
		for (int i = 0 ; i < MAX_CLIENTS ; i++){
			//socket descriptor
			sd = client_socket[i];
			
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET(sd, &readfds);
			
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}

		//wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
		int ret = select(max_sd + 1 , &readfds , NULL , NULL , NULL);

		if (ret < 0)
			std::cerr << "Select error" << std::endl; // continue ???
		 
		//If something happened on the master socket, then its an incoming connection
		int new_socket;
		if (FD_ISSET(master_socket, &readfds)){
			if ((new_socket = accept(master_socket, (struct sockaddr *)&addr, (socklen_t*)&addrlen)) < 0){
				std::cerr << "Accept failed" << std::endl;
				exit(EXIT_FAILURE); // ???
			}
		 
			//inform user of socket number - used in send and receive commands
			std::cout << "New connection, socket fd is " << new_socket << ", ip is : " 
				<< inet_ntoa(addr.sin_addr) << ", port : " 
				<< ntohs(addr.sin_port) << std::endl;

			//send new connection greeting message
			if (send(new_socket, message.c_str(), message.size(), 0) < 0)
				std::cerr << "Send failed" << std::endl;

			//add new socket to array of sockets
			for (int i = 0; i < MAX_CLIENTS; i++){
				//if position is empty
				if( client_socket[i] == 0){
					client_socket[i] = new_socket;
					std::cout << "Adding to list of sockets as " << i << std::endl;
					break;
				}
			}
		}

		
		//else its some IO operation on some other socket (write, except)
		int valread;
		// std::string tmp = "<html><header></header><body>Hello World!!!</body></html>";
		for (int i = 0; i < MAX_CLIENTS; i++){
			sd = client_socket[i];
			 
			if (FD_ISSET(sd, &readfds)){
				//Check if it was for closing, and also read the incoming message
				if ((valread = read(sd , buffer, 1024)) == 0){
					//Somebody disconnected, get his details and print
					getpeername(sd, (struct sockaddr*)&addr, (socklen_t*)&addrlen);
					std::cout << "Host disconnected, ip " << inet_ntoa(addr.sin_addr) << ", port "
						<< ntohs(addr.sin_port) << std::endl;
					
					//Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
				}
				//Echo back the message that came in
				else{
					//set the string terminating NULL byte on the end of the data read
					std::cout << buffer << std::endl;
					buffer[valread] = '\0';
					send(sd, buffer, std::strlen(buffer), 0);
				}
			}
		}
	}

	return 0;
}