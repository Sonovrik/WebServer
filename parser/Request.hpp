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
	// headers
	std::map<std::string,std::string>	_headers;
	// query string
	std::map<std::string,std::string>	_queryString;
	// path info
	std::string					_pathInfo;
	// file extension
	std::string					_fileExtension;
	// body
	std::string					_body;

	// errors
	int							_statusCode;
	// constants
	static std::string const	_methodsNames[];
	static int const			_numMethods;

public:
	Request();
	~Request();
	Request(Request const &);
	Request	&operator=(Request const &);

	// parse request
	bool	parseStartLine(std::string);
	bool	parseBody(std::string);
	bool	parseHeaders(std::string);
	bool	setHeader(std::string);
	bool	checkHeaderName(std::string);
	bool	checkHeaderValue(std::string);
	bool	checkRepeatHeader(std::pair<std::string, std::string>);
	void	trimString(std::string &);
	bool	parseQueryString();

	// setters
	void	setMethod(std::string);
	void	setPath(std::string);
	void	setVersion(std::string);
	void	setBody(std::string);
	void	setHeaders(std::map<std::string,std::string>);
	void	setStatusCode(int);

	// getters
	std::string const	&getMethod(void) const;
	std::string	const	&getPath(void) const;
	std::string	const	&getVersion(void) const;
	std::string	const	&getBody(void) const;
	int const		 	&getStatusCode(void) const;
	std::map<std::string,std::string> const &getHeaders(void) const;
	std::map<std::string,std::string> const &getQueryString(void) const;
	std::string const	&getPathInfo(void) const;
	std::string const	&getFileExtension(void) const;
};

#endif