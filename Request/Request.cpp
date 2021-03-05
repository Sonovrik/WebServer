#include "Request.hpp"

Request::Request():
	_method(""),
	_path(""),
	_version(""),
	_body(""),
	_pathInfo("cgi_tester"),
	_statusCode(200){}

Request::~Request(){}

Request::Request(Request const &other) {
	*this = other;
}

Request &Request::operator=(Request const &other) {
	if (this != &other){
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_statusCode = other._statusCode;
		this->_queryString = other._queryString;
	}
	return *this;
}

// setters

void	Request::setMethod(std::string method) {
	this->_method = method;
}

void	Request::setPath(std::string path) {
	this->_path = path;
}

void	Request::setVersion(std::string version) {
	this->_version = version;
}

void	Request::setBody(std::string body) {
	this->_body = body;
}

void	Request::setHeaders(std::map<std::string,std::string> map) {
	this->_headers = map;
}

void	Request::setStatusCode(int statusCode) {
	this->_statusCode = statusCode;
}

void	Request::setPathInfo(std::string pathInfo) {
	this->_pathInfo = pathInfo;
}

// getters

std::string const	&Request::getMethod(void) const {
	return this->_method;
}

std::string const	&Request::getPath(void) const {
	return this->_path;
}


std::string const	&Request::getServerName(void) const {
	return this->_path;
}

std::string const	&Request::getVersion(void) const {
	return this->_version;
}

std::string const	&Request::getBody(void) const {
	return this->_body;
}

int const 			&Request::getStatusCode(void) const {
	return this->_statusCode;
}

std::map<std::string,std::string> const &Request::getHeaders(void) const {
	return this->_headers;
}


std::string const &Request::getQueryString(void) const {
	return this->_queryString;
}

std::string const	&Request::getPathInfo(void) const {
	return this->_pathInfo;
}


const std::string Request::_methodsNames[] = {"GET", "HEAD",
			"POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};
const int Request::_numMethods = 8;

bool	Request::parseQueryString() {
	size_t		pos;
	std::string	query;
	std::pair<std::string, std::string> node;

	if ((pos = this->_path.find("?")) == std::string::npos)
		return true;
	this->_queryString = this->_path.substr(pos + 1, this->_path.length());
	std::cout << this->_queryString << std::endl;
	return true;
}

bool	Request::parseStartLine(std::string str) {
	std::string	token[3];
	size_t		pos;

	// parse parts of string in tokens
	for (int i = 0; i < 2; i++) {
		if ((pos = str.find(' ')) == std::string::npos)
			return false;
		if (pos == 0)
			return false;
		token[i] = str.substr(0, pos);
		str.erase(0, pos + 1);
	}
	token[2] = str.substr(0, str.length());
	if (token[2].empty())
		return false;
	// find methods
	for (int i = 0; i < _numMethods; i++) {
		if (_methodsNames[i] == token[0])
			this->_method = token[0];
	}
	if (this->_method.empty())
		return false;
	this->_path = token[1];

	// please add envs in this string
	if (token[2] != "HTTP/1.1\r\n")
		return false;
	this->_version = token[2].substr(0, token[2].length() - 2);
	// parse query string
	if (!parseQueryString())
		return false;
	// cut http://
	if ((pos = this->_path.find("http://")) != std::string::npos)
		this->_path = this->_path.substr(7, this->_path.length() - 7);
	return true;
}

// clear ws from the begining and the end
void		Request::trimString(std::string &line) {
	int i = 0;
	while (line[i] == ' ')
		i++;
	line.erase(0, i);
	i = line.length() - 1;
	while (line[i] == ' ')
		i--;
	line.erase(i + 1, line.length());
}

bool		Request::checkHeaderName(std::string name) {
	if (!name.length())
		return false;
	for (int i = 0; i < name.length(); i++) {
		if (!(name[i] >= 33 && name[i] <= 126))
			return false;
	}
	return true;
}

bool		Request::checkHeaderValue(std::string value) {
	for (int i = 0; i < value.length(); i++) {
		if (!isascii(value[i]))
			return false;
	}
	return true;
}

bool		Request::checkRepeatHeader(std::pair<std::string, std::string> node) {
	if (this->_headers.find(node.first) != this->_headers.end()) {
		if (node.first == "HOST")
			return false;
		if (this->_headers.find(node.first)->second == node.second)
			return false;
		if (this->_headers.find(node.first)->first == "TRANSFER-ENCODING"
			&& node.first == "CONTENT-LENGTH")
			return false;
	}
	return true;
}

bool		Request::setHeader(std::string line) {
	size_t		pos = 0;
	
	if ((pos = line.find(':')) == std::string::npos)
		return false;

	std::pair<std::string, std::string> node;
	node.first = line.substr(0, pos);
	for (int i = 0; i < node.first.length(); i++) {
		node.first[i] = std::toupper(node.first[i]);
	}
	if (!checkHeaderName(node.first))
		return false;
	line.erase(0, pos + 1);
	trimString(line);

	node.second = line;
	if (!checkHeaderValue(node.second))
		return false;
	if (!checkRepeatHeader(node))
		return false;
	// content-length >= 0
	if (node.first == "CONTENT-LENGTH" && atoi(node.second.c_str()) < 0)
		return false;
	// exchange header if it repeats
	if (this->_headers.find(node.first) != this->_headers.end())
		this->_headers.find(node.first)->second = node.second;
	else
		this->_headers.insert(node);
	// std::cout << this->_headers.find(node.first)->first << ":" << this->_headers.find(node.first)->second << std::endl;
	return true;
}

bool		Request::parseHeaders(std::string req) {
	std::string	tmp;
	size_t		pos = 0;
	std::string	delimenter = "\r\n";

	while ((pos = req.find(delimenter)) != std::string::npos && pos != req.length() && pos != 0) {
		tmp = req.substr(0, pos);
		if (!(setHeader(tmp)))
			return false;
		req.erase(0, pos + delimenter.size());
	}

	// if end and no host
	// if ((pos = req.find("\r\n")) != std::string::npos)
	// 	if (this->_headers.find("HOST") == this->_headers.end())
	// 		return false;

	// if (this->_headers.find("HOST") == this->_headers.end())
	// 	return false;
	// if ((pos = req.find("\r\n")) == std::string::npos)
	// 	return false;
	return true;
}

bool		Request::parseBody(std::string req) {
	size_t pos = req.find("\r\n\r\n");

	if (pos + 4 != req.length())
		_body = req.substr(pos + 4, req.length());
	// else
		// std::cout << "waiting body" << std::endl;
	return true;
}

// Request		parseRequest(std::string req) {
// 	size_t		pos = 0;
// 	Request		request;

// 	if ((pos = req.find("\r\n")) == std::string::npos) {
// 		request.setStatusCode(400);
// 		return request;
// 	}
// 	std::string	tmp(req.substr(0, pos + 2));
// 	if (request.parseStartLine(tmp) == false) {
// 		request.setStatusCode(400);
// 		return request;
// 	}

// 	req.erase(0, pos + 2);

// 	if (request.parseHeaders(req) == false) {
// 		request.setStatusCode(400);
// 		return request;
// 	}
// 	if (request.getMethod() == "POST") {
// 		if (request.parseBody(req) == false) {
// 			request.setStatusCode(400);
// 			return request;
// 		}
// 	}
// 	return request;
// }

Request		parseRequest(std::string req) {
	size_t		pos = 0;
	Request		request;

	if ((pos = req.find("\r\n")) == std::string::npos) {
		request.setStatusCode(400);
		return request;
	}
	std::string	tmp(req.substr(0, pos + 2));
	if (request.parseStartLine(tmp) == false) {
		request.setStatusCode(400);
		return request;
	}

	req.erase(0, pos + 2);

	if (request.parseHeaders(req) == false) {
		request.setStatusCode(400);
		return request;
	}
	if (request.getMethod() == "POST") {
		if (request.parseBody(req) == false) {
			request.setStatusCode(400);
			return request;
		}
	}
	return request;
}

// int main() {
// 	Request req;
	
// 	std::string tmp2 = "GET / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\n\r\n";
// 	req = parseRequest(tmp2);

// 	std::cout << req.getBody() << std::endl;
// 	std::cout << req.getStatusCode() << std::endl;

// 	return 0;
// }