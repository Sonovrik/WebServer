#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <string>
#include <map>
#include <vector>


class Request{

private:
	// start line
	std::string		_method;
	std::string		_path;
	std::string		_version;
	static std::string const _methodsNames[]; 

	// headers
	std::map<std::string,std::string>	_headers;
	// body
	std::string		_body;

public:
	Request();
	~Request();
	Request(Request const &);
	Request &operator=(Request const &);


	// parse request
	bool	parse_start_line(std::string);
	bool	parse_headers(std::string);
	bool	push_map_node(std::string line);

	// setters
	void	set_method(std::string);
	void	set_path(std::string);
	void	set_version(std::string);
	void	set_body(std::string);

	void	set_headers(std::map<std::string,std::string>);

	// getters

	std::string const	&get_method(void) const;
	std::string	const	&get_path(void) const;
	std::string	const	&get_version(void) const;
	std::string	const	&get_body(void) const;
};

#endif