#include "Client.hpp"

Client::Client():
	_sd(0),
	_flag(WAIT){}

Client::Client(int sd):
	_sd(sd),
	_flag(WAIT){}

Client::~Client() {}

void	Client::setStatusCode(int code){
	this->_statusCode = code;
}

void	Client::setSd(int sd){
	this->_sd = sd;
}

void	Client::setFlag(int flag){
	this->_flag = flag;
}

void	Client::setRequest(Request &reqv){
	this->_request = reqv;
}

int		Client::getSd(void) const{
	return this->_sd;
}

int		Client::getFlag(void) const{
	return this->_flag;
}

Request	&Client::getRequest(void){
	return this->_request;
}

int		Client::getStatusCode(void) const{
	return this->_statusCode;
}

const std::string &Client::getPathToFile() const {
	return pathToFile;
}

void Client::setPathToFile(const std::string &pathToFile) {
	this->pathToFile = pathToFile;
}

int Client::getWhere() const {
	return this->where;
}

void Client::setWhere(int where) {
	this->where = where;
}
