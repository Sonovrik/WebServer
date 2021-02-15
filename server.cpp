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
#include <unistd.h>
#include <sys/select.h>

bool flag = false;

void	sigint(int signal){
	flag = true;

}

int		main(void){
	signal(SIGINT, sigint);
	int	socketfd;
	socklen_t	sizeofaddr;
	// Create a socket
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0){
		std::cout << "Error creating socket\n" << std::endl;	
		return 0;
	}

	// Init addr
	sockaddr_in addr; //= {0};
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8000);
	addr.sin_family = AF_INET;
	sizeofaddr = sizeof(addr);
	
	// Bind
	bind(socketfd, (sockaddr *)&addr, sizeofaddr);


	// listen
	listen(socketfd, SOMAXCONN);

	// new Socket to connection
	int socketcon;
	socketcon = accept(socketfd, (sockaddr *)&addr, &sizeofaddr);

	if (socketcon == 0){
		std::cout << "Error connection" << std::endl;
	}
	else{
		std::cout << "Client Connected" << std::endl;
	}
	// map на заголовки 
	// nc для подключения
	// разобарться с селектом и потом читать стандарт по обработке запросов
	// CGI
	// написать на curl для запросов в браузер с терминала ???


	fd_set readfds;
	int master_socket = socketfd;
	int max_clients = 10;
	while(true){
		// clear socket set
		FD_ZERO(&readfds);

		// add master socket to set
		FD_SET(master_socket, &readfds);
	}
	char msg[3] = {0};
	int res;
	while (!flag){
		int res = recv(socketcon, msg, sizeof(msg), 0);
		if (res == 0){
			std::cout << "Disconnected" << std::endl;
			break;
		}
		if (res == -1){
			std::cout << "Error in recv()" << std::endl;
			break;
		}
		write(1, msg, std::strlen(msg));
		msg[0] = 0;
		msg[1] = 0;
		msg[2] = 0;
	}
	int fd = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &fd, sizeof(int)); // break TCP conn
	return 0;
}