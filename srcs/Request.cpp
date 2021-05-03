#include "Request.hpp"

const std::string Request::_methodsNames[] = {"GET", "HEAD",
											  "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};
const int Request::_numMethods = 8;

Request::Request():
		_method(""),
		_path(""),
		_version(""),
		_body(""),
		_queryString(""),
		_pathInfo(""),
		_return(WAIT),
		_toClose(false),
		_bodyLen(0),
		_waitBody(false),
		_buffer("") {}

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
		this->_buffer = other._buffer;
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
	_pathInfo = "";
	_headers.clear();
	_body = "";
	_toClose = false;
	_return = WAIT;
	_bodyLen = 0;
	_waitBody = false;
}

bool	Request::parseQueryString() {
	size_t		pos;

	if ((pos = this->_path.find("?")) == std::string::npos)
		return true;
	this->_queryString = this->_path.substr(pos + 1, this->_path.length());
	return true;
}

bool	Request::parseStartLine(std::string &str) {
	std::string	token[3];
	size_t		pos;

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
	for (int i = 0; i < _numMethods; i++) {
		if (_methodsNames[i] == token[0])
			this->_method = token[0];
	}
	if (this->_method.empty())
		return false;
	this->_path = token[1];
	if (token[2] != "HTTP/1.1\r\n")
		return false;
	this->_version = token[2].substr(0, token[2].length() - 2);
	parseQueryString();
	return true;
}

bool		Request::checkHeaderName(std::string const &name) const{
	if (!name.length())
		return false;
	for (int i = 0; i < name.length(); i++) {
		if (!(name[i] >= 33 && name[i] <= 126))
			return false;
	}
	return true;
}

bool		Request::checkHeaderValue(std::string const &value) const {
	for (int i = 0; i < value.length(); i++) {
		if (!isascii(value[i]))
			return false;
	}
	return true;
}

bool		Request::checkHeader(std::pair<std::string, std::string> &node) {
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
	if (node.first == "CONNECTION" && node.second.find("close") != std::string::npos)
		this->_toClose = true;
	return true;
}

bool		Request::setHeader(std::string &line) {
	size_t		pos = 0;
	std::pair<std::string, std::string> node;

	if ((pos = line.find(':')) == std::string::npos)
		return false;
	node.first = line.substr(0, pos);
	for (int i = 0; i < node.first.length(); i++)
		node.first[i] = std::toupper(node.first[i]);
	if (!checkHeaderName(node.first))
		return false;
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
	return true;
}

bool		Request::parseHeaders(std::string &req) {
	std::string	tmp;
	size_t		pos = 0;
	std::string	delimiter = "\r\n";

	if (this->_waitBody)
		return true;
	while ((pos = req.find(delimiter)) != std::string::npos && pos != req.length() && pos != 0) {
		tmp = req.substr(0, pos);
		if (!(setHeader(tmp)))
			return false;
		req.erase(0, pos + delimiter.size());
	}
	pos = req.find("\r\n");
	if (pos == std::string::npos && req[0] != '\0') {
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
			this->_return = REQUEST_END;
		else
			this->_waitBody = true;
	}
	return true;
}

bool		Request::parseBody(std::string &req) {
	size_t pos;
	// wait for body
	if (req.empty())
		return true;
	// chunked
	if (this->_headers.find("TRANSFER-ENCODING")->second.find("chunked") != std::string::npos) {
		while ((pos = req.find("\r\n")) != std::string::npos) {
			// parse chunk value
			if (req.substr(0, pos) == "0") {
				if (req.find("\r\n\r\n") == std::string::npos) {
					this->_buffer = req;
					return true ;
				}
			}
			if (this->_bodyLen == 0 || req.substr(0, pos) == "0") {
				try {
					this->_bodyLen = stoul(req.substr(0, pos), NULL, 16);
					req.erase(0, pos + 2);
				}
				catch (std::exception &e) {
					return true;
				}
			}
			// end
			if (this->_bodyLen == 0) {
				this->_return = REQUEST_END;
				this->_waitBody = false;
				return true;
			}
			// parse chunk
			if (this->_bodyLen > 0) {
				pos = req.find("\r\n");
				if (pos != std::string::npos && req.substr(0, pos).length() == this->_bodyLen ) {
					this->_body.append(req.substr(0, pos));
					req.erase(0, pos + 2);
					this->_bodyLen = 0;
				}
				else {
					this->_buffer = req;
					req.erase();
					return true;
				}
			}
		}
		if (req[0] != '\0') {
			this->_buffer = req;
			req.erase();
		}
		return true;
	}
	// content length
	if (this->_headers.find("CONTENT-LENGTH") != this->_headers.end()) {
		if (_bodyLen == 0)
			this->_bodyLen = atoi(this->_headers.find("CONTENT-LENGTH")->second.c_str());
		if ((pos = req.find("\r\n")) != std::string::npos) {
			if (req.substr(0, pos).length() >= this->_bodyLen) {
				this->_body.append(req.substr(0, this->_bodyLen));
				req.erase(0, pos + 2);
				this->_bodyLen = 0;
			}
			if (req.substr(0, pos).length() < this->_bodyLen) {
				this->_body.append(req.substr(0, pos));
				this->_bodyLen -= pos;
				req.erase(0, pos + 2);
			}
			if (_bodyLen == 0)
				this->_return = REQUEST_END;
		}
		else if (req.length() == this->_bodyLen) {
			this->_body.append(req);
			this->_bodyLen = 0;
			req.erase();
			this->_return = REQUEST_END;
		}
	}
	else {
		this->_return = ERR_LENGTH_REQUIRED;
		return false;
	}
	return true;
}

// return REQUEST_END / WAIT / ERR_BAD_REQUEST / ERR_LENGTH_REQUIRED
int			parseRequest(std::string req, Request &request, int maxBodySize) {
	size_t		pos = 0;

	if (request.getBuffer() != "") {
		req.insert(0, request.getBuffer());
		request.setBuffer("");
	}
	if ((pos = req.find("\r\n")) == std::string::npos) {
		request.setBuffer(req);
		req.erase();
		return request.getReturn();
	}
	if (request.getMethod() == "") {
		pos = req.find("\r\n");
		std::string	tmp(req.substr(0, pos + 2));
		if (!request.parseStartLine(tmp)) {
			request.setReturn(ERR_BAD_REQUEST);
			return request.getReturn();
		}
		req.erase(0, pos + 2);
	}
	if (!request.getWaitBody() && !request.parseHeaders(req)) {
		request.setReturn(ERR_BAD_REQUEST);
		return request.getReturn();
	}
	if ((request.getMethod() == "POST" || request.getMethod() == "PUT") && request.getWaitBody()) {
		if (!request.parseBody(req))
			return request.getReturn();
		else if (request.getBody().length() > maxBodySize) {
			request.setReturn(ERR_TOO_LARGE_BODY);
			return request.getReturn();
		}
	}
	return request.getReturn();
}