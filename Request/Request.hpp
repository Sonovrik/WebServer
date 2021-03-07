#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <string>
#include <map>
#include <vector>

#define	WAIT	0
#define SEND	1
#define ERROR	-1

class Request{

private:
	// start line
	std::string					_method;
	std::string					_path;
	std::string					_version;
	std::string					_queryString;
	std::string					_pathInfo;

	std::map<std::string,std::string>	_headers;
	// body
	std::string					_body;

	bool						_toClose;
	int							_return;
	unsigned long				_chunk;
	bool						_waitBody;
	// constants
	static std::string const	_methodsNames[];
	static int const			_numMethods;

public:
	Request();
	~Request();
	Request(Request const &);
	Request	&operator=(Request const &);

	void	reset();
	// parse request
	bool	parseStartLine(std::string);
	bool	parseBody(std::string);
	bool	parseHeaders(std::string &);
	bool	setHeader(std::string);
	bool	checkHeaderName(std::string);
	bool	checkHeaderValue(std::string);
	bool	checkHeader(std::pair<std::string, std::string>);
	void	trimString(std::string &);
	bool	parseQueryString();

	// setters
	void	setMethod(std::string);
	void	setPath(std::string);
	void	setVersion(std::string);
	void	setBody(std::string);
	void	setHeaders(std::map<std::string,std::string>);
	void	setPathInfo(std::string);
	void	setReturn(int);
	void	setToClose(int toClose);

	// getters
	std::string const	&getMethod(void) const;
	std::string	const	&getPath(void) const;
	std::string	const	&getVersion(void) const;
	std::string const	&getServerName(void) const;
	std::string	const	&getBody(void) const;
	std::map<std::string,std::string> const &getHeaders(void) const;
	std::string const 	&getQueryString(void) const;
	std::string const	&getPathInfo(void) const;
	int					getReturn(void) const;
	bool				getToClose(void) const;
	bool				getWaitBody(void) const;

};

int			parseRequest(std::string req, Request &request);

#endif