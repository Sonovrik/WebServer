#include "Request.hpp"

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
// "TRANSFER-ENCODING" - <длина блока в HEX><CRLF><содержание блока><CRLF> last: 0<CRLF><CRLF>
// "USER-AGENT" the browser sending the request
// "WWW-AUTHENTICATE" - response header defines the authentication method that should be used to gain access to a resource; WWW-Authenticate: Basic realm="Access to the staging site", charset="UTF-8"
// "ACCEPT"


// ?chunked \r\n

// Quality value
// q=0.2 (0 - 1)
// default = 1. 0 - not acceptable, 0.001 least preferred
//  A sender of qvalue MUST NOT generate more than three digits after the decimal point.  User configuration of these values ought to be limited in the same fashion.

const std::string Request::_methodsNames[] = {"GET", "HEAD",
			"POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};
const int Request::_numMethods = 8;

Request::Request():
	_method(""),
	_path(""),
	_version(""),
	_body(""),
	_queryString(""),
	_pathInfo("cgi_tester"),
	_return(WAIT),
	_toClose(false),
	_bodyLen(0),
	_waitBody(false) {}

Request::~Request(){}

Request::Request(Request const &other) {
	*this = other;
}

Request &Request::operator=(Request const &other) {
	if (this != &other){
		this->_method = other._method;
		this->_path = other._path;
		this->_pathInfo = other._pathInfo;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_queryString = other._queryString;
		this->_toClose = other._toClose;
		this->_return = other._return;
		this->_bodyLen = other._bodyLen;
		this->_waitBody = other._waitBody;
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

void	Request::setPathInfo(std::string pathInfo) {
	this->_pathInfo = pathInfo;
}
void	Request::setReturn(int ret) {
	this->_return = ret;
}

void	Request::setToClose(int toClose) {
	this->_toClose = toClose;
}

void	Request::setQueryString(int queryString) {
	this->_queryString = queryString;
}	

void	Request::setBodyLen(int bodyLen) {
	this->_bodyLen = bodyLen;
}

void	Request::setWaitBody(int waitBody) {
	this->_waitBody = waitBody;
}

void	Request::setBuffer(std::string buffer) {
	this->_buffer = buffer;
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

std::map<std::string,std::string> const &Request::getHeaders(void) const {
	return this->_headers;
}

std::string const &Request::getQueryString(void) const {
	return this->_queryString;
}

std::string const	&Request::getPathInfo(void) const {
	return this->_pathInfo;
}

int					Request::getReturn(void) const {
	return this->_return;
}

bool				Request::getToClose(void) const {
	return this->_toClose;
}

bool				Request::getWaitBody(void) const {
	return this->_waitBody;
}

std::string			Request::getBuffer(void) const {
	return this->_buffer;
}

// methods

void	Request::reset() {
	_buffer = "";
	_method = "";
	_path = "";
	_version = "";
	_queryString = "";
	_pathInfo = "cgi_tester";
	_headers.clear();
	_body = "";
	_toClose = false;
	_return = WAIT;
	_bodyLen = 0;
	_waitBody = false;
}

bool	Request::parseQueryString() {
	size_t		pos;
	std::string	query;
	std::pair<std::string, std::string> node;

	if ((pos = this->_path.find("?")) == std::string::npos)
		return true;
	this->_queryString = this->_path.substr(pos + 1, this->_path.length());
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

bool		Request::checkHeader(std::pair<std::string, std::string> node) {
	if (this->_headers.find(node.first) != this->_headers.end()) {
		if (node.first == "HOST")
			return false;
		if (this->_headers.find(node.first)->second == node.second)
			return false;
	}
	if ((this->_headers.find("TRANSFER-ENCODING") != this->_headers.end()
		&& node.first == "CONTENT-LENGTH") || (this->_headers.find("CONTENT-LENGTH") != this->_headers.end()
		&& node.first == "TRANSFER-ENCODING"))
		return false;
	if (node.first == "CONTENT-LENGTH") {
		// content-length >= 0
		if (atoi(node.second.c_str()) < 0) {
			this->_toClose = true;
			return false;
		}
		if (this->_headers.find(node.first)->first == "CONTENT-LENGTH" && this->_headers.find(node.first)->second != node.second) {
			this->_toClose = true;
			return false;
		}
	}
	if (node.first == "CONNECTION" && node.second.find("close") != std::string::npos) {
		this->_toClose = true;
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
	// if (!checkHeaderName(node.first))
	// 	return false;
	line.erase(0, pos + 1);
	trimString(line);
	node.second = line;
	if (!checkHeaderValue(node.second))
		return false;
	if (!checkHeader(node))
		return false;
	// exchange header if it repeats
	if (this->_headers.find(node.first) != this->_headers.end())
		this->_headers.find(node.first)->second = node.second;
	else
		this->_headers.insert(node);
	// std::cout << "|" << node.first << "|" << node.second << "|" << std::endl;
	return true;
}

bool		Request::parseHeaders(std::string &req) {
	std::string	tmp;
	size_t		pos = 0;
	std::string	delimenter = "\r\n";

	if (this->_waitBody)
		return true;
	std::cout << "{" << ((pos = req.find(delimenter)) != std::string::npos) << "}";
	std::cout << "{" << (int)req.find(delimenter) << "}";
	std::cout << "{" << (pos == 0) << "}" << std::endl;
	while ((pos = req.find(delimenter)) != std::string::npos && pos != req.length() && pos != 0) {
		tmp = req.substr(0, pos);
		if (!(setHeader(tmp)))
			return false;
		req.erase(0, pos + delimenter.size());
	}
	pos = req.find("\r\n");
	// std::cout << "{" << pos << "}" << std::endl;
	if (pos == std::string::npos && !req.empty()) {
		// std::cout << "P" << std::endl;
		this->_buffer = req;
		req.erase();
	}
	// if end and no host
	else if (pos != std::string::npos && this->_headers.find("HOST") == this->_headers.end()) {
		this->_toClose = true;
		return false;
	}
	else if (pos == 0) {
		req.erase(0, 2);
		if (this->_method != "POST" && this->_method != "PUT")
			this->_return = SEND;
		else
			this->_waitBody = true;
		// std::cout << "here" << std::end;
	}
	return true;
}

bool		Request::parseBody(std::string req) {
	size_t pos;
	// wait for body
	if (req.empty())
		return true;
	// chunked
	if (this->_headers.find("TRANSFER-ENCODING")->second.find("chunked") != std::string::npos) {
		while ((pos = req.find("\r\n")) != std::string::npos) {
			// parse chunk value
			if (this->_bodyLen == 0) {
				try {
					this->_bodyLen = stoul(req.substr(0, pos), NULL, 16);
					req.erase(0, pos + 2);
				}
				catch (std::exception &e) {
					this->_return = ERR_BAD_REQUEST;
					return false;
				}
			}
			// end
			if (this->_bodyLen == 0) {
				this->_return = SEND;
				this->_waitBody = false;
				return true;
			}
			// parse chunk
			if (this->_bodyLen > 0) {
				if ((pos = req.find("\r\n")) != std::string::npos) {
					if (req.substr(0, pos).length() >= this->_bodyLen) {
						this->_body = this->_body + req.substr(0, this->_bodyLen);
						req.erase(0, pos + 2);
						this->_bodyLen = 0;
					}
					if (req.substr(0, pos).length() < this->_bodyLen) {
						this->_body = this->_body + req.substr(0, pos);
						this->_bodyLen -= pos;
						req.erase(0, pos + 2);
					}
				}
			}
		}
		return true;
	}
	// content length
	if (this->_headers.find("CONTENT-LENGTH") != this->_headers.end()) {
		if (_bodyLen == 0)
			this->_bodyLen = atoi(this->_headers.find("CONTENT-LENGTH")->second.c_str());
		if ((pos = req.find("\r\n")) != std::string::npos) {
			if (req.substr(0, pos).length() >= this->_bodyLen) {
					this->_body = this->_body + req.substr(0, this->_bodyLen);
					req.erase(0, pos + 2);
					this->_bodyLen = 0;
			}
			if (req.substr(0, pos).length() < this->_bodyLen) {
				this->_body = this->_body + req.substr(0, pos);
				this->_bodyLen -= pos;
				req.erase(0, pos + 2);
			}
			if (_bodyLen == 0)
				this->_return = SEND;
		}
	}
	else {
		this->_return = ERR_LENGTH_REQUIRED;
		return false;
	}
	return true;
}

// return SEND / WAIT / ERR_BAD_REQUEST / ERR_LENGTH_REQUIRED
int			parseRequest(std::string req, Request &request) {
	size_t		pos = 0;

	if (!request.getBuffer().empty()) {
		req = request.getBuffer() + req;
		request.setBuffer("");
	}
	if ((pos = req.find("\r\n")) == std::string::npos) {
		request.setBuffer(req);
		return request.getReturn();
	}
	// else if (pos == 0)
	// 	return request.getReturn();
	std::cout << "|" << req << "|" << std::endl;
	if (request.getMethod() == "") {
		pos = req.find("\r\n");
		std::string	tmp(req.substr(0, pos + 2));
		if (request.parseStartLine(tmp) == false) {
			request.setReturn(ERR_BAD_REQUEST);
			std::cout << "start line problem" << std::endl;
			return request.getReturn();
		}
		req.erase(0, pos + 2);
	}
	if (request.getWaitBody() == false && request.parseHeaders(req) == false) {
		std::cout << "header problem" << std::endl;
		return ERR_BAD_REQUEST;
	}
	if ((request.getMethod() == "POST" || request.getMethod() == "PUT") && request.getWaitBody() == true) {
		if (request.parseBody(req) == false) {
			return request.getReturn();
		}
		// std::cout << "BODY: |" << request.getBody() << "|" << std::endl;
	}
	return request.getReturn();
}
