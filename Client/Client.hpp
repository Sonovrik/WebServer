#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"

#define	OK		0
#define CLOSED	1
#define ERROR	2

class Client {

private:
	int		_sd;
	int		_flag;
	Request _request;

public:
	Client();
	Client(int sd);
	~Client();

	int		getSd(void) const;
	int		getFlag(void) const;
	Request	getRequest(void) const;

};

#endif