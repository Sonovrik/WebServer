#include "Response.hpp"

Response::Response():
	_version(""),
	_statusCode(0),
	_statusMessage(""),
	_body(""){}


void	Response::setBadRequest(Server const &serv){
	this->_version = serv.getEnvValue("SERVER_PROTOCOL");
	this->_statusMessage = "Bad Request";

	_headers.insert(std::make_pair("Connection", serv.getEnvValue("CONNECTION")));
	_headers.insert(std::make_pair("Content-Language", "en"));
	_headers.insert(std::make_pair("Content-Type", "text/html"));
	if (serv.get_errorPage().empty()){
		this->_body = "<html>\n\n    <head>\n        <title>404 Not Found</title>\n    </head>"
			"\n    <body>\n        <p align=\"center\" style=\"font-size:50px\">\n"
			"            404 Not Found\n        </p>\n    </body>\n</html>";
	}
	else{
		std::ifstream	in(serv.get_errorPage());
		std::string		tmp("");
		while (getline(in, tmp)){
			this->_body.append(tmp);
		}
	}
	_headers.insert(std::make_pair("Content-Length", std::to_string(_body.size())));
	// _headers.insert(std::make_pair(""));
}

Response::Response(int code, Server const &serv):
	_version(""),
	_statusCode(code),
	_statusMessage(""),
	_body(""){
		if (this->_statusCode == 400){
			setBadRequest(serv);
		}
	}


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

void	Response::set_statusCode(int statusCode){
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
std::string 	Response::get_version(void) const{
	return _version;
}

int			Response::get_statusCode(void) const{
	return _statusCode;
}

std::string		Response::get_statusMessage(void) const{
	return _statusMessage;
}

std::map<std::string,std::string>		&Response::get_headers(void){
	return _headers;
}

std::string		Response::get_body(void) const{
	return _body;
}
