#include "Request.hpp"


Request::Request():
	_method(""),
	_path(""),
	_version(""),
	_body(""){}

Request::~Request(){}

Request::Request(Request const &other){
	if (this != &other){
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
	}
}

Request &Request::operator=(Request const &other){
	if (this != &other){
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return *this;
}

// setters
void	Request::set_method(std::string method){
	this->_method = method;
}

void	Request::set_path(std::string path){
	this->_path = path;
}

void	Request::set_version(std::string version){
	this->_version = version;
}

void	Request::set_body(std::string body){
	this->_body = body;
}

void	Request::set_headers(std::map<std::string,std::string> map){
	this->_headers = map;
}

// getters
std::string const	&Request::get_method(void) const{
	return _method;
}

std::string const	&Request::get_path(void) const{
	return _path;
}

std::string const	&Request::get_version(void) const{
	return _version;
}

std::string const	&Request::get_body(void) const{
	return _body;
}