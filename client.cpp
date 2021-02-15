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

#define SOCKET_ERROR -1

int		main(void){
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
	
	// New sock to connection
	int sockcon = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sockcon, (sockaddr *)&addr, sizeofaddr) != 0){
		std::cout << "Error: failed to connection" << std::endl;
		return 0;
	}
	std::cout << "Connection is ok!" << std::endl;

	// receive message
	char msg[2] = {0};
	int res;
	while (true){
		res = recv(sockcon, msg, sizeof(msg), 0);
		write(1, msg, std::strlen(msg));
		// std::cout << msg << res << std::endl;

		if (res == 0){
			std::cout << "Disconnected" << std::endl; // ???
			break;
		}
		if (res == SOCKET_ERROR){
			std::cout << "Error in recv()" << std::endl;
			break;
		}
		
	}
	return 0;

}