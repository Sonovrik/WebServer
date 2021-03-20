#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"
#include <iostream>
// #include <map>
// #include <vector>

#define	toCGI		0
#define	toServer	1

class Client {

private:
	int		_where;
	int		_sd;
	int		_flag;
	int		_statusCode;
	Request _request;
	std::string pathToFile;
public:
	Client();
	Client(int sd);
	~Client();

	void	setSd(int sd);
	void	setFlag(int flag);
	void	setRequest(Request &reqv);
	void	setStatusCode(int code);
	void	setPathToFile(const std::string &pathToFile);
	void 	setWhere(int where);
	void	setToClose(bool toClose);

	int		getStatusCode(void) const;
	int		getSd(void) const;
	int		getFlag(void) const;
	Request	&getRequest(void);
	const	std::string &getPathToFile() const;
	int 	getWhere() const;
	bool	getToClose() const;

};

#endif