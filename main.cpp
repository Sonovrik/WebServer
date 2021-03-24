#include "Server.hpp"
#include "./Request/Request.hpp"
#include "./Client/Client.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "./ConfigParser/ConfigParser.hpp"
#include "Response.hpp"
#include "CGI/CGI.hpp"
#include "CGI/RequestConfigMatch.hpp"

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

	try{
		ConfigParser parser;
		if (!parser.parseConfig("webserv.conf")){
			std::cerr << "Some parser error!!!" << std::endl;
			return -1;
		}
		initMimeTypes();
		timeval time;
		time.tv_sec = 10;
		time.tv_usec = 0;
		std::vector<Server>::iterator it;
		std::vector<Server> serversList(parser.getServers());

		it = serversList.begin();
		for (; it != serversList.end(); it++){
			it->create_master_socket(AF_INET, SOCK_STREAM, 0);
			it->set_address_socket(it->get_ip().c_str(), atoi(it->get_port().c_str()), AF_INET);
			it->bind_master_socket();
			listen_socket(it->get_master_socket(), SOMAXCONN);
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

			int	ret = select(max_sd + 1, &readfds, &writefds, NULL, &time);
			if (ret == 0){
				std::cout << "Time out" << std::endl;
				continue;
			}
			else if (ret < 0){
				std::cout << "asd" << std::endl;
				throw std::exception();
			}
			
			it = serversList.begin();
			for (; it != serversList.end(); it++){
				if (FD_ISSET(it->get_master_socket(), &readfds)){
					int new_connection = it->accept_master_socket();
					fcntl(new_connection, F_SETFL, O_NONBLOCK);
					it->add_client(new_connection);
				}
				for (size_t i = 0; i < it->get_clientCount(); i++){
					int		sd = it->get_clientsd(i + 1);
					std::string buf(1024, '\0');
					Client &client = it->get_Client(i + 1);
					if (client.getFlag() != SEND && FD_ISSET(sd, &readfds)){
						if ((ret = read(sd, (void *)buf.c_str(), 1024)) == 0){
							client.clear();
							it->delete_client(i + 1);
							close(client.getSd());
							client.setSd(0);
							break;
						}
						else {
							client.setFlag(parseRequest(buf, client.getRequest()));
							if (client.getFlag() == WAIT) {
								std::cout << "wait" << std::endl;
								break;
							}
							else if (client.getFlag() == ERR_BAD_REQUEST || client.getFlag() == ERR_LENGTH_REQUIRED) {
								// no response - will status code be rewritten after response creation
								if (client.getFlag() == ERR_LENGTH_REQUIRED)
									client.setStatusCode(411);
								if (client.getFlag() == ERR_BAD_REQUEST)
									client.setStatusCode(400);
								client.setFlag(SEND);
							}
						}
					}
					else if (client.getFlag() == SEND && FD_ISSET(sd, &writefds)) {
						if (client.getStatusCode() == 200) {
							if (RequestConfigMatch(client, *it) == toCGI) {
								try {
									CGI qqq(client.getRequest(), *it);
									qqq.init(client.getRequest(), *it);
									qqq.creatENV();
									qqq.exec();
								}
								catch (std::exception &exception) {
                                    std::cout << "ggggg" << exception.what() << std::endl;
									setErrorCode(exception.what(), client);
                                    return 1;
								}
							}
//							else
//								std::cerr << "server exec" << std::endl;
						}
						Response resp(*it, client);
						send(client.getSd(), resp.getResponse().c_str(), resp.get_respSize(), 0);
						client.clear();
						if (client.getToClose()){
							it->delete_client(i + 1);
							close(client.getSd());
							client.setSd(0);
						}
						break;
					}
				}
			}
		}
	}
	catch(const std::exception& e){
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}