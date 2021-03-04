#include "Server.hpp"
#include <unistd.h>
#include "./ConfigParser/ConfigParser.hpp"

int		findMaxSD(std::vector<Server> &servers){
	std::vector<Server>::iterator it = servers.begin();
	int maxSd = 0;
	for (; it != servers.end(); it++){
		if (it->get_maxSd() > maxSd)
			maxSd = it->get_maxSd();
	}
	return maxSd;
}

int main(){
	ConfigParser parser;
	if (!parser.parseConfig("webserv.conf")){
		std::cerr << "Some parser error!!!" << std::endl;
		return -1;
	}
	std::vector<Server>::iterator it;
	std::vector<Server> serversList(parser.getServers());

	it = serversList.begin();
	for (; it != serversList.end(); it++){
		//create a master socket
		(*it).create_master_socket(AF_INET, SOCK_STREAM, 0);

		//type of socket created
		(*it).set_address_socket((*it).get_ip().c_str(), atoi((*it).get_port().c_str()), AF_INET);

		//bind the socket to localhost port
		(*it).bind_master_socket();

		//try to specify maximum of 128 pending connections for the master socket
		listen_socket((*it).get_master_socket(), SOMAXCONN);
		
		// add envs
		(*it).fullConfigEnvironment();
	}
	
	fd_set	readfds;
	fd_set	writefds;
	int		max_sd = 0;
	
	bool loop = true;
	while (loop){
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		for (it = serversList.begin(); it != serversList.end(); it++){
			FD_SET(it->get_master_socket(), &readfds);
			// FD_SET(it->get_master_socket(), &writefds);
			it->FD_reset(&readfds);
			it->FD_reset(&writefds);
		}
		max_sd = findMaxSD(serversList);
	
		
		int	ret = select(max_sd + 1, &readfds, &writefds, NULL, NULL); // 10 sec
		if (ret == 0){
			std::cout << "Time out" << std::endl;
			continue;
		}
		else if (ret < 0){
			// throw something;
			std::cerr << "Select error" << std::endl;
			return -1;
		}
		
		it = serversList.begin();
		for (; it != serversList.end(); it++){
			if (FD_ISSET(it->get_master_socket(), &readfds)){
				int new_connection = it->accept_master_socket();
				it->add_sd(new_connection);
			}
		}
	
	}
	
		// for (int i = 0; i < MAX_CLIENTS; i++){
		// 	int		sd = serv.get_client_sd(i);
		// 	int		ret;
		// 	char	buffer[1024];
		// 	if (FD_ISSET(sd, &readfds)){
		// 		// Check if it closing or incoming message
		// 		if ((ret = read(sd, buffer, 1024)) == 0){
		// 			std::cout << "Host disconnected, sd " << sd << std::endl;
		// 			close(sd);
		// 			serv.set_client_socket(i, 0);
		// 		}
		// 		// just read message and do some   
		// 		else{
		// 			buffer[ret] = '\0';
		// 			if (buffer[ret - 2] == '\r')
		// 			std::cout << "asd123" << std::endl;
					
		// 			std::cout << buffer << std::endl;

		// 		}
		// 	}
		// }
	return 0;
}