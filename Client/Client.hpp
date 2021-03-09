#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"
#include <iostream>
#include <map>
#include <vector>


class Client {

private:
	int		_where;
	int		_sd;
	int		_flag;
	int		_statusCode;
	Request _request;

public:
	Client();
	Client(int sd);
	~Client();

	void	setSd(int sd);
	void	setFlag(int flag);
	void	setRequest(Request &reqv);
	void	setStatusCode(int code);

	int		getStatusCode(void) const;
	int		getSd(void) const;
	int		getFlag(void) const;
	Request	&getRequest(void);

};

#endif