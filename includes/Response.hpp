#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "Server.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class	Response{

private:
	std::string		_version;
	int				_statusCode;
	std::string		_statusMessage;
	std::map<std::string, std::string>	_headers;
	std::string		_body;
	bool			_toClose;


	// exec
	void	setError(Server const &, Client const &client);
	void	execPUT(Server const &serv, Client &client);
	void	execGET(Server const &serv, Client &client);
	void	parseCgiFile();
	void 	execAfterCGI(Server const &serv, Client &client);
	void	execListing(Server const &serv, Client &client);

	void	cmpFileAndBody(Client &client) const;
	void	writeBodyInFile(Client &client) const;


	// headers setters
	void	set_headers(std::map<std::string,std::string>);
	void	setContentLocation(Client &client);
	void	setContentType(std::string const &pathToFile);
	void	setLastModified(std::string const &file);
	void	setDate();
	void	setContentLength(std::string const &length);

	void	set_statusMessage(std::string);
	void	set_body(std::string);


public:
	Response();
	Response(Server const &serv, Client &client);
	~Response();
	Response(Response const &);
	Response &operator=(Response const &);


	// setters
	void	set_version(std::string);
	void	setStatusCode(int);
	std::string	setStatusMessage(int);

	// getters
	std::string 	get_version(void) const;
	int				get_statusCode(void) const;
	std::string		get_statusMessage(void) const;
	std::map<std::string,std::string>	&get_headers(void);
	std::string		get_body(void) const;

	std::string		createResponse(void) const;

};


#endif