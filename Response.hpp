#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "Server.hpp"
#include "utils.hpp"

class	Response{

private:
	std::string		_version;
	int				_statusCode;
	std::string		_statusMessage;
	std::map<std::string, std::string>	_headers;
	std::string		_body;
	bool			_toClose;
	size_t			_respSize;

public:
	Response();
	Response(Server const &serv, Client &client);
	~Response();
	Response(Response const &);
	Response &operator=(Response const &);

	void		setError(Server const &);

	// setters
	void	set_version(std::string);
	void	setStatusCode(int);
	// void	set_statusMessage(int code);
	std::string	setStatusMessage(int);

	void	set_statusMessage(std::string);
	void	set_headers(std::map<std::string,std::string>);
	void	set_body(std::string);

	void	set_date();

	// getters
	std::string 	get_version(void) const;
	int				get_statusCode(void) const;
	std::string		get_statusMessage(void) const;
	std::map<std::string,std::string>	&get_headers(void);
	std::string		get_body(void) const;
	size_t			get_respSize(void) const;

	std::string		getResponse(void);
};


#endif