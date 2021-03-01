#include "Request.hpp"


Request::Request():
	_method(""),
	_path(""),
	_version(""),
	_body(""),
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

// getters

std::string const	&Request::getMethod(void) const {
	return this->_method;
}

std::string const	&Request::getPath(void) const {
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

