#include "Response.hpp"

// Allow: GET, HEAD <- 405 Method not allowed
// Connection: alive/close
// Content-Language/Content-Length/Content-Location/Content-Type
// Date
// Last-Modified
// Location <- redirection or when a new resource has been created
// Retry-After: 120 / Fri, 07 Nov 2014 23:59:59 GMT | seconds/ HTTP date
// Server
// Transfer-Encoding: chunked / compress / deflate / gzip / identity

// WWW-Authenticate: Indicates the authentication scheme that should be used to access the requested entity. WWW-Authenticate: Basic
// 401 (Unauthorized) -> WWW-Authenticate
// A server MAY generate a WWW-Authenticate header field in other response messages to indicate that supplying credentials (or different credentials) might affect the response.


Response::Response():
	_version("HTTP/1.1"),
	_statusCode(200),
	_respSize(0),
	_statusMessage("ОК"),
	_body(""),
	_toClose(false){}

// void	Response::set_statusMessage(int code){
// 	switch(code){
// 		case 400:
// 			this->_statusMessage = "Bad Request";
// 			break;
// 		case 401:
// 			this->_statusMessage = "Payment Required";
// 			break;
// 		case 403:
// 			this->_statusMessage = "Forbidden";
// 			break;
// 		case 404:
// 			this->_statusMessage = "Not Found";
// 			break;
// 		case 405:
// 			this->_statusMessage = "Method Not Allowed";
// 			break;
// 		case 406:
// 			this->_statusMessage = "Not Acceptable";
// 			break;
// 		case 408:
// 			this->_statusMessage = "Request Timeout";
// 			break;
// 		case 409:
// 			this->_statusMessage = "Conflict";
// 			break;
// 		case 410:
// 			this->_statusMessage = "Gone";
// 			break;
// 		case 411:
// 			this->_statusMessage = "Length Required";
// 			break;
// 		case 413:
// 			this->_statusMessage = "Payload Too Large";
// 			break;
// 		case 414:
// 			this->_statusMessage = "URI Too Long";
// 			break;
// 		case 415:
// 			this->_statusMessage = "Unsupported Media Type";
// 			break;
// 		case 417:
// 			this->_statusMessage = "Expectation Failed";
// 			break;
// 		case 426:
// 			this->_statusMessage = "Upgrade Required";
// 			break;
// 		default:
// 			break;
// 	}
// }

// add all messages
std::string	Response::setStatusMessage(int code){
	switch(code){
		case 400: return "Bad Request";
		case 401: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 413: return"Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 417: return "Expectation Failed";
		case 426: return "Upgrade Required";
		default:
			return "OK";
	}
}
Response::~Response(){}

void	Response::setError(Server const &serv) {
	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));

	_headers.insert(std::make_pair("Content-Language", "en"));
	_headers.insert(std::make_pair("Content-Type", "text/html"));
	if (serv.get_errorPath(this->_statusCode).empty()){
		this->_body = get_errorPage(this->_statusCode);
	}
	else{
		std::ifstream	in(serv.get_errorPath(this->_statusCode));
		std::string		tmp("");
		while (getline(in, tmp)){
			this->_body.append(tmp + "\n");
		}
	}
	_headers.insert(std::make_pair("Content-Length", std::to_string(_body.size())));
	set_date();
}

void	Response::execPut(Client &client) {
	struct stat st;
	std::string tmp("");
	std::fstream	file(client.getPathToFile());
	std::string fileContent("");
	// int fd = open(client.getPathToFile(), O_RDONLY || O_WRONLY || O_APPEND);

	if (file.is_open()) {
		std::string	tmp("");
		while (getline(file, tmp))
			fileContent.append(tmp + "\n");
		if (fileContent == client.getRequest().getBody()) {
			// set_LastModified(client.getPathToFile());
		}
		else {
			file.clear(); // clear eof flag
			// + seek to 0
			
        }
	}

	// int ret = open();

}

void	Response::execGET(Client &client){
	std::ifstream	file(client.getPathToFile());
	std::string		tmp("");
	if (file.is_open())
		throw std::exception();
	while (getline(file, tmp)){
		this->_body.append(tmp);
	}
	this->_headers.insert(std::make_pair("Content-Length", std::to_string(this->_body.size())));
	if (client.getMethod() == "HEAD")
		_body.clear();

	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));
	
	set_date();

}

Response::Response(Server const &serv, Client &client):
	_version(serv.getEnvValue("SERVER_PROTOCOL")),
	_statusCode(client.getStatusCode()),
	_respSize(0),
	_statusMessage(setStatusMessage(client.getStatusCode())),
	_body(""),
	_toClose(client.getToClose()) {
		if (client.getMethod() == "GET" || client.getMethod() == "HEAD")
			execGET(client);
		if (client.getMethod() == "PUT")
			execGET(client);

}

size_t			Response::get_respSize(void) const {
	return _respSize;
}

std::string		Response::getResponse(void) {
	std::string		ret("");
	ret.append(this->_version);
	ret.append(" " + std::to_string(this->_statusCode));
	ret.append(" " + this->_statusMessage);
	ret.append("\r\n");
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	for (; it != _headers.end(); it++){
		ret.append(it->first + ": " + it->second + "\r\n");
	}
	ret.append("\r\n");
	if (!_body.empty()){
		ret.append(this->_body);
		ret.append("\r\n");
	}
	this->_respSize = ret.size();
	return ret;
}

Response::Response(Response const &other) {
	*this = other;
}

Response	&Response::operator=(Response const &other) {
	if (this != &other){
		this->_version = other._version;
		this->_statusCode = other._statusCode;
		this->_statusMessage = other._statusMessage;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_toClose = other._toClose;
		this->_respSize = other._respSize;
	}
	return *this;
}

// setters
void	Response::set_version(std::string version) {
	this->_version = version;
}

void	Response::setStatusCode(int statusCode) {
	this->_statusCode = statusCode;
	this->_statusMessage = setStatusMessage(statusCode);
}

void	Response::set_statusMessage(std::string statusMessage) {
	this->_statusMessage = statusMessage;
}

void	Response::set_headers(std::map<std::string,std::string> headers) {
	this->_headers = headers;
}

void	Response::set_body(std::string body) {
	this->_body = body;
}

void	Response::set_LastModified(std::string &file) {
	struct stat st;
	time_t rawtime;
	struct tm	*timeinfo;
	char		buffer[30];

	// stat(file.c_str(), &st);
	// // st.
	// std::ctime(&st.st_mtime);
	// timeinfo = localtime(&(time_t)(st.st_mtimespec));
	// strftime (buffer,30,"%a, %d %b %G %T %Z",timeinfo);
	// std::cout << buffer << std::endl;
}

void	Response::set_date(){
	time_t		rawtime;
	struct tm	*timeinfo;
	char		buffer[30];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime (buffer,30,"%a, %d %b %G %T %Z",timeinfo);
	this->_headers.insert(std::make_pair("Date", buffer));

	// use stat
	this->_headers.insert(std::make_pair("Last-Modified", buffer)); 
}

// getters
std::string 	Response::get_version(void) const {
	return _version;
}

int			Response::get_statusCode(void) const {
	return _statusCode;
}

std::string		Response::get_statusMessage(void) const {
	return _statusMessage;
}

std::map<std::string,std::string>		&Response::get_headers(void) {
	return _headers;
}

std::string		Response::get_body(void) const {
	return _body;
}
