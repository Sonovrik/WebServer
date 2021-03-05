

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>

class	Response{

private:
	std::string		_version;
	std::string		_statusCode;
	std::string		_statusMessage;
	std::map<std::string, std::string>	_headers;
	std::string		_body;

public:
	Response();
	~Response();
	Response(Response const &);
	Response &operator=(Response const &);

	// setters
	void	set_version(std::string);
	void	set_statusCode(std::string);
	void	set_statusMessage(std::string);
	void	set_headers(std::map<std::string,std::string>);
	void	set_body(std::string);

	// getters
	std::string const	&get_version() const;
	std::string	const	&get_statusCode() const;
	std::string	const	&get_statusMessage() const;
	std::map<std::string,std::string>	const	&get_headers() const;
	std::string	const	&get_body() const;
};




#endif