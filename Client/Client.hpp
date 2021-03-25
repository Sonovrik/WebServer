#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"
#include <iostream>

#define	toCGI		1
#define	toServer	0

class Client {

private:
	int		_where;
	int		_sd;
	int		_flag;
	int		_locPos;
	int		_statusCode;
	Request	_request;
	std::string	pathToFile;

public:

	// Coplin
	Client();
	Client(int sd);
	~Client();

	// setters
	void	setSd(int sd);
	void	setFlag(int flag);
	void	setRequest(Request &reqv);
	void	setStatusCode(int code);
	void	setPathToFile(const std::string &pathToFile);
	void 	setWhere(int where);
	void	setToClose(bool toClose);
	void	setLocPos(int pos);

	// getters
	std::string const	&getMethod(void) const;
	int		getStatusCode(void) const;
	int		getSd(void) const;
	int		getFlag(void) const;
	Request	&getRequest(void);
	const	std::string &getPathToFile() const;
	int 	getWhere() const;
	bool	getToClose() const;
	int		getLocPos(void) const;

	// Update Methods
	void	clear(void);
};

bool operator==(const Client &c1, const Client &c2);


#endif