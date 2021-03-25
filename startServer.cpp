#include "ConfigParser/ConfigParser.hpp"
#include "Server.hpp"
#include "Client/Client.hpp"
#include "Response.hpp"
#include "CGI/CGI.hpp"
#include "CGI/RequestConfigMatch.hpp"

static int		findMaxSD(std::vector<Server> &servers){
	std::vector<Server>::iterator it = servers.begin();
	int maxSd = 0;
	for (; it != servers.end(); it++){
		if (it->get_maxSd() > maxSd)
			maxSd = it->get_maxSd();
	}
	return maxSd;
}

static std::vector<Server>		initServers(std::string const &config){
	ConfigParser parser;
	parser.parseConfig(config);

	std::vector<Server> serversList(parser.getServers());
	std::vector<Server>::iterator it = serversList.begin();
	for (; it != serversList.end(); it++){
		it->create_masterSocket(AF_INET, SOCK_STREAM, 0);
		it->set_addressSocket(it->get_ip().c_str(), atoi(it->get_port().c_str()), AF_INET);
		it->bind_masterSocket();
		it->listen_socket(SOMAXCONN);
		it->fullConfigEnvironment();
	}

	return serversList;
}

static int		resetServers(fd_set *readfds, fd_set *writefds, std::vector<Server> &serversList){
	std::vector<Server>::iterator it = serversList.begin();
	FD_ZERO(readfds);
	FD_ZERO(writefds);

	for (; it < serversList.end(); it++){
		FD_SET(it->get_master_socket(), readfds);
		it->FD_reset(readfds);
		it->FD_reset(writefds);
	}
	return findMaxSD(serversList);
}

static void		readRequest(Server &serv, Client &client){
	std::string buf(atoi(serv.get_maxBufferSize().c_str()), '\0');
	int ret;
	std::cout << buf.size() << std::endl;
	if ((ret = read(client.getSd(), (void *)buf.c_str(), buf.size())) == 0){
		client.clear();
		serv.delete_client(client);
		close(client.getSd());
		client.setSd(0);
	}
	else{
		client.setFlag(parseRequest(buf, client.getRequest(), serv.get_maxBodySize()));
		if (client.getFlag() == ERR_BAD_REQUEST || client.getFlag() == ERR_LENGTH_REQUIRED
		|| client.getFlag() == ERR_TOO_LARGE_BODY) {
			if (client.getFlag() == ERR_LENGTH_REQUIRED)
				client.setStatusCode(411);
			if (client.getFlag() == ERR_BAD_REQUEST)
				client.setStatusCode(400);
			if (client.getFlag() == ERR_TOO_LARGE_BODY)
				client.setStatusCode(413);
			client.setFlag(SEND);
		}
	}
}

static void		sendResponse(Server &serv, Client &client){
	if (client.getStatusCode() == 200){
		if (RequestConfigMatch(client, serv) == toCGI) {
			try {
				CGI qqq(client.getRequest(), serv);
				qqq.init(client.getRequest(), serv);
				qqq.creatENV();
				qqq.exec();
			}
			catch (std::exception &exception) {
				std::cout << "ggggg" << exception.what() << std::endl;
				setErrorCode(exception.what(), client);
			}
		}
	}
	Response resp(serv, client);
	if (resp.get_respSize() > 2048){
		// std::string message = resp.getResponse().substr()
	}
	send(client.getSd(), resp.getResponse().c_str(), resp.get_respSize(), 0);
	client.clear();
	if (client.getToClose()){
		serv.delete_client(client);
		close(client.getSd());
		client.setSd(0);
	}
}

static void		startServer(std::vector<Server> &serversList){
	fd_set	readfds;
	fd_set	writefds;
	int		max_sd = 0;
	bool	loop = true;
	bool	flag = false;
	int		ret = 0;
	std::vector<Server>::iterator it;

	timeval time;
	time.tv_sec = 10;
	time.tv_usec = 0;
	while (loop){
		try{
			max_sd = resetServers(&readfds, &writefds, serversList);
			if ((ret = select(max_sd + 1, &readfds, &writefds, NULL, &time)) == 0
				|| ret < 0)
				continue;
			it = serversList.begin();
			flag = false;
			for (; it != serversList.end(); it++){
				if (FD_ISSET(it->get_master_socket(), &readfds))
					it->addNewConnection();
				for (size_t i = 0; i < it->get_clientCount(); i++){
					Client &client = it->get_Client(i + 1);
					int		sd = it->get_clientsd(i + 1);
					sd = client.getSd();
					if (client.getFlag() != SEND && FD_ISSET(sd, &readfds)){
						readRequest(*it, client);
						flag = true;
						break;
					}
					else if (client.getFlag() == SEND && FD_ISSET(sd, &writefds)){
						sendResponse(*it, client);
						flag = true;
						break;
					}
				}
				if (flag)
					break;
			}
		}
		catch (std::exception &e){
			std::cerr << e.what() << std::endl;
			continue;
		}
	}
}

int		main(){
	std::vector<Server>	serversList;

	try{
		serversList = initServers("webserv.conf");
		initMimeTypes();
		startServer(serversList);
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}