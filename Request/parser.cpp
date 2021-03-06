// valid name + valid value
// check is there the same name+field in Request
// Accept: application/json
// Accept: application/xml
// no headers
// no value
// Не допускается никаких пробелов между именем поля заголовка и двоеточием
// required Host
// ? post without body -> error
// IF REQUEST HEADERS DOES NOT END WITH \r\n
// ? must headers always end with \r\n\r\n
// if the same header and not the same value -> exchange
// Content-Length >= 0
// Content-Length + Transfer-Encoding -> false
// check header name (> 1 symbols 33-126)
// no header value -> valid
// cut http://

// "ACCEPT-CHARSET"
// "ACCEPT-LANGUAGE"
// "ALLOW" - allowed methods
// "AUTHORIZATION" Authorization: <type> <credentials>.  "Authorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l"  - (base64 encoded) / "Authorization: Basic username:password"
// "CONTENT-LANGUAGE"
// "CONTENT-LENGTH"
// "CONTENT-LOCATION" an alternate location for the returned data
// "CONTENT-TYPE"
// "DATE"
// "HOST" - the host and port number of the server to which the request is being sent. Host: <host>:<port> (host=domain name of the server, post=TCP port number on which the server is listening.)
// "LAST-MODIFIED"
// "LOCATION" response header indicates the URL to redirect a page to. It only provides a meaning when served with a 3xx (redirection) or 201 (created) status response.
// "REFERER" содержит URL исходной страницы, с которой был осуществлен переход на текущую страницу Referer: <url>
// "RETRY-AFTER"
// "SERVER" the software used by the origin server that handled the request — that is, the server that generated the response. Server: <product>. Server: Apache/2.4.1 (Unix). <product> The name of the software or product that handled the request. Usually in a format similar to User-Agent
// "TRANSFER-ENCODING"
// "USER-AGENT" the browser sending the request
// "WWW-AUTHENTICATE"
// "ACCEPT"

#include "Request.hpp"

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
	if (this->_headers.find("HOST") == this->_headers.end())
		return false;
	if ((pos = req.find("\r\n")) == std::string::npos)
		return false;
	return true;
}

bool		Request::parseBody(std::string req) {
	size_t pos = req.find("\r\n\r\n");

	if (pos + 4 != req.length())
		_body = req.substr(pos + 4, req.length());
	else
		return false;
	return true;
}

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

int main() {
	Request req;
	// std::string tmp1 = "POST /cgi-bin/process.cgi HTTP/1.1\n\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n\nHost: www.example.com\n\nContent-Type: application/x-www-form-urlencoded\n\n";
	// req = parseRequest(tmp1);
	
	// std::string tmp2 = "POST / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\n\r\n";
	// req = parseRequest(tmp2);

	// std::string tmp2 = "GET http://localhost:8080/path?name=value&name=777 HTTP/1.1\r\nHost: 127.0.0.1:5991\r\n\r\n";
	// req = parseRequest(tmp2);

	// std::string tmp3 = "GET / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*  \r\nNewHeader:123\r\n\r\n";
	// req = parseRequest(tmp3);
	// std::cout << req.getStatusCode() << std::endl;
	// std::cout << std::endl;

	// std::string tmp4 = "GET / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept:\r\n\r\n";
	// req = parseRequest(tmp4);
	// std::cout << req.getStatusCode() << std::endl;
	// std::cout << std::endl;

	// std::string tmp5 = "GET / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*  \r\n:123\r\n\r\n";
	// req = parseRequest(tmp5);
	// std::cout << req.getStatusCode() << std::endl;
	// std::cout << std::endl;

	// std::string tmp6 = "GET / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*  \r\nhh\n hh:123\r\n\r\n";
	// req = parseRequest(tmp6);
	// std::cout << req.getStatusCode() << std::endl;
	// std::cout << std::endl;

	// std::string tmp3 = "GET   HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*   \r\n\r\n";
	// req = parseRequest(tmp3);

	// std::string tmp3 = "\r\n";
	// req = parseRequest(tmp3);

	// std::string tmp6 = "POST /cgi-bin/process.cgi?name=value;name1=value1&name2=value2;name3=value3 HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nsay=Hi&to=Mom";
	// req = parseRequest(tmp6);

	// std::string tmp7 = "POST /cgi-bin/process.cgi?name=value;name1=value1&name2=value2;=value3;name4= HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nsay=Hi&to=Mom";
	// req = parseRequest(tmp7);

	// std::string tmp6 = "POST HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nsay=Hi&to=Mom";
	// req = parseRequest(tmp6);

	// std::string tmp4 = "POST / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\n\r\nsay=Hi&to=Mom";
	// req = parseRequest(tmp4);

	// ??? IF REQUEST DOES NOT END WITH \r\n
	// std::string tmp8 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\n    Accept: */*";
	// req = parseRequest(tmp8);

	// wrong delemeters
	// std::string tmp5 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\n\nUser-Agent: curl/7.47.0\r\n    Accept: */*    \r\n\r\n";
	// req = parseRequest(tmp5);
	
	// - double accept
	// std::string tmp4 = "GET /index.html HTTP/1.1\r\nUser-Agent: curl/7.47.0\r\n    Accept: */*    \r\n    Accept: */*    \r\n\r\n";
	// req = parseRequest(tmp4);

	// - no host
	// std::string tmp9 = "POST / HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\n\r\nsay=Hi&to=Mom";
	// req = parseRequest(tmp9);

 	// // no body with post
	// std::string tmp9 = "POST / HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\n\r\n";
	// req = parseRequest(tmp9);
	
	// double Host
	// std::string tmp3 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nHost:1233\r\nUser-Agent: curl/7.47.0\r\n    Accept: */*   \r\n\r\n";
	// req = parseRequest(tmp3);

	// std::cout << "!" << req.getMethod() << "!" << std::endl;
	// std::cout << "!" << req.getVersion() << "!" << std::endl;
	// std::cout << "!" << req.getPath() << "!" << std::endl;
	// std::cout << "!" << req.getQueryString() << "!" << std::endl;

	std::cout << req.getStatusCode() << std::endl;

	return 0;
}