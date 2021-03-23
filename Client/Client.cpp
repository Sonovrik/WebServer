#include "Client.hpp"

Client::Client():
	_sd(0),
	_flag(WAIT),
	_where(toServer),
	_statusCode(200) {}

Client::Client(int sd):
	_sd(sd),
	_flag(WAIT),
	_where(toServer),
	_statusCode(200) {}

Client::~Client() {}

// setters
void	Client::setStatusCode(int code) {
	this->_statusCode = code;
}

void	Client::setSd(int sd) {
	this->_sd = sd;
}

void	Client::setFlag(int flag) {
	this->_flag = flag;
}

void	Client::setRequest(Request &reqv) {
	this->_request = reqv;
}

void	Client::setWhere(int where) {
	this->_where = where;
}

void	Client::setToClose(bool toClose) {
	this->_request.setToClose(toClose);
}

// getters
int		Client::getSd(void) const {
	return this->_sd;
}

int		Client::getFlag(void) const {
	return this->_flag;
}

Request	&Client::getRequest(void) {
	return this->_request;
}

int		Client::getStatusCode(void) const {
	return this->_statusCode;
}

const std::string &Client::getPathToFile() const {
	return pathToFile;
}

void Client::setPathToFile(const std::string &pathToFile) {
	this->pathToFile = pathToFile;
}

int Client::getWhere() const {
	return this->_where;
}

bool	Client::getToClose(void) const {
	return this->_request.getToClose();
}

std::string const	&Client::getMethod(void) const{
	return this->_request.getMethod();
}

void	Client::clear(void){
	// if (getToClose() == true){
	// 	this->_sd = 0;
	// }
	this->_where = toServer;
	this->_flag = WAIT;
	this->_statusCode = 200;
	this->_request.reset();
	this->pathToFile.clear();
}

