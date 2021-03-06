#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"
#include <iostream>
#include <map>
#include <vector>


class Client {

private:
	int		where;
	int		_sd;
	int		_flag;
	Request _request;

public:
	Client();
	Client(int sd);
	~Client();

	void	setSd(int sd);
	void	setFlag(int flag);
	void	setRequest(Request &reqv);

	int		getSd(void) const;
	int		getFlag(void) const;
	Request	&getRequest(void);

};

#endif