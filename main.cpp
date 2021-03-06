#include "Server.hpp"
// #include "./Request/Request.hpp"
#include "./Client/Client.hpp"
#include <unistd.h>
#include <fcntl.h>
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
		it->create_master_socket(AF_INET, SOCK_STREAM, 0);

		//type of socket created
		it->set_address_socket(it->get_ip().c_str(), atoi(it->get_port().c_str()), AF_INET);

		//bind the socket to localhost port
		it->bind_master_socket();

		//try to specify maximum of 128 pending connections for the master socket
		listen_socket(it->get_master_socket(), SOMAXCONN);
		
		// add envs
		it->fullConfigEnvironment();
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
				fcntl(new_connection, F_SETFL, O_NONBLOCK);
				// it->add_sd(new_connection);
				it->add_client(new_connection); // add client
			}
			for (size_t i = 0; i < it->get_clientCount(); i++){
				int		sd = it->get_clientsd(i + 1);
				std::string buf(1024, '\0');
				if (FD_ISSET(sd, &readfds)){
					if ((ret = read(sd, (void *)buf.c_str(), 1024)) == 0){
						close(sd);
						it->delete_client(i + 1);
					}
					else {
						Client &tmp = it->get_Client(i + 1);
						tmp.setFlag(parseRequest(buf, tmp.getRequest()));
						if (tmp.getFlag() == ERROR){
						// 	Response resp(400); //fill all atributes
						// 	tmp.setFlag(SEND);
						// 	// create error response;
							std::cout << "To close:" << std::boolalpha << tmp.getRequest().getToClose() << std::endl;
							std::cout << "error" << std::endl;
						}
						else if (tmp.getFlag() == WAIT){
						// 	continue;
							std::cout << "wait" << std::endl;
						}
						if (tmp.getFlag() == SEND){
						// 	server.exec_reqv();
						// 	tmp.getwhere() == CGI
						// 		bulina;
						// 	else
								
						// 	server.getStatus_code();
						// 	Response();
						// 	Response resp(200);
							std::cout << "send" << std::endl;
						}
						std::cout << buf << std::endl;
					}
				}
				if (FD_ISSET(sd, &writefds)){
					// if (tmp.getFlag() == SEND){
					// 	send(response.get_response());
					// }
					continue;
					// if (write == yes){
					// 	send();
					// }
					// writefds.fds_bits;
					send(sd, "Asd\r\n\r\n", 7, 0);
					// delete sd writefds;
				}
			}
		}
	}
	return 0;
}