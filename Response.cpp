#include "Response.hpp"

Response::Response():
	_version(""),
	_statusCode(""),
	_statusMessage(""){}


Response::~Response(){}


Response::Response(Response const &other){
	if (this != &other){
		this->_version = other._version;
		this->_statusCode = other._statusCode;
		this->_statusMessage = other._statusMessage;
	}
}


Response	&Response::operator=(Response const &other){
	if (this != &other){
		this->_version = other._version;
		this->_statusCode = other._statusCode;
		this->_statusMessage = other._statusMessage;
	}
	return *this;
}


// setters
void	Response::set_version(std::string version){
	this->_version = version;
}

void	Response::set_statusCode(std::string statusCode){
	this->_statusCode = statusCode;
}

void	Response::set_statusMessage(std::string statusMessage){
	this->_statusMessage = statusMessage;
}

void	Response::set_headers(std::map<std::string,std::string> headers){
	this->_headers = headers;
}

void	Response::set_body(std::string body){
	this->_body = body;
}


// getters
std::string const	&Response::get_version() const{
	return _version;
}

std::string	const	&Response::get_statusCode() const{
	return _statusCode;
}

std::string	const	&Response::get_statusMessage() const{
	return _statusMessage;
}

std::map<std::string,std::string>	const	&Response::get_headers() const{
	return _headers;
}

std::string	const	&Response::get_body() const{
	return _body;
}
