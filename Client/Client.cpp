#include "Client.hpp"

Client::Client() : _flag(OK) {}

Client::~Client() {}

int		Client::getSd(void) const {
	return this->_sd;
}
int		Client::getFlag(void) const {
	return this->_flag;
}
Request	Client::getRequest(void) const {
	return this->_request;
}