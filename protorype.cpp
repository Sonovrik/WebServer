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

void		allow_mul_cons_socket(int socket){
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

int main(void)
{
	int			master_socket, activity, valread, sd;
	int			client_socket[MAX_CLIENTS] = {0};
	socklen_t	addrlen;
	int			max_sd;
	sockaddr_in	addr;
	 
	char buffer[1025];  //data buffer of 1K
	 
	//set of socket descriptors
	fd_set readfds;

	//a message
	char *message = "ECHO Daemon v1.0 \r\n";

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

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	 
	//accept the incoming connection
	addrlen = sizeof(addr);
	puts("Waiting for connections ...");
	
	while(true) 
	{
		//clear the socket set
		FD_ZERO(&readfds);
 
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		
		//add child sockets to set
		for (int i = 0 ; i < MAX_CLIENTS ; i++) 
		{
			//socket descriptor
			sd = client_socket[i];
			
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
			
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
 
		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
   
		if ((activity < 0) && (errno!=EINTR)) 
		{
			printf("select error");
		}
		 
		//If something happened on the master socket , then its an incoming connection
		int new_socket;
		if (FD_ISSET(master_socket, &readfds)) 
		{
			if ((new_socket = accept(master_socket, (struct sockaddr *)&addr, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
		 
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));
	   
			//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
			{
				perror("send");
			}
			 
			puts("Welcome message sent successfully");
			 
			//add new socket to array of sockets
			for (int i = 0; i < MAX_CLIENTS; i++) 
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
					
					break;
				}
			}
		}
		 
		//else its some IO operation on some other socket :)
		for (int i = 0; i < MAX_CLIENTS; i++) 
		{
			sd = client_socket[i];
			 
			if (FD_ISSET( sd , &readfds)) 
			{
				//Check if it was for closing , and also read the incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&addr , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));
					 
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
				 
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end of the data read
					buffer[valread] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}
	 
	return 0;
}