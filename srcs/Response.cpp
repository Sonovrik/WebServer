#include "Response.hpp"

Response::Response():
		_version("HTTP/1.1"),
		_statusCode(200),
		_statusMessage("ОК"),
		_body(""),
		_toClose(false){}

std::string	Response::setStatusMessage(int code){
	switch(code){
		case 201: return "Created";
		case 301: return "Moved Permanently";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 417: return "Expectation Failed";
		case 426: return "Upgrade Required";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";
		default:
			return "OK";
	}
}
Response::~Response(){}

void	Response::setError(Server const &serv, Client const &client) {
	this->_statusCode = client.getStatusCode();
	this->_body = serv.get_errorPath(this->_statusCode);
	if (this->_statusCode == 500)
		this->_toClose = true;
	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));
	_headers.insert(std::make_pair("Content-Language", "en"));
	_headers.insert(std::make_pair("Content-Type", "text/html"));
	_headers.insert(std::make_pair("Content-Length", to_string(_body.size())));
	_headers.insert(std::make_pair("Server", serv.get_serverName()));
	if (this->_statusCode == 401)
		_headers.insert(std::make_pair("WWW-Authenticate", "Basic realm=\"" + serv.get_locations()[client.getLocPos()]._directives.find("auth_basic")->second + "\", charset=\"UTF-8\""));
	if (this->_statusCode == 405)
		_headers.insert(std::make_pair("Allow", serv.getLocationMethods(client.getLocPos())));
	setDate();
}

void	Response::cmpFileAndBody(Client &client) const{
	int ret;
	int fd;
	char buf[10240];
	std::string fileContent("");
	if ((fd = open(client.getPathToFile().c_str(), O_RDWR, 0666)) == -1)
		throw std::runtime_error("500");
	while ((ret = read(fd, (void *)buf, (10240 - 1))) > 0){
		buf[ret] = '\0';
		fileContent.append(buf);
	}
	if(ret == -1){
		close(fd);
		throw std::runtime_error("500");
	}
	if (fileContent == client.getRequest().getBody())
		client.setStatusCode(204);
	close(fd);
}

void	Response::writeBodyInFile(Client &client) const{
	int fd;
	int ret;

	if ((fd = open(client.getPathToFile().c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0666)) == -1)
		throw std::runtime_error("500");
	std::string str = client.getRequest().getBody();
	while ((ret = write(fd, str.c_str(), str.size())) > 0)
		str.erase(0, ret);
	close(fd);
	if(ret == -1)
		throw std::runtime_error("500");
}

void	Response::execPUT(Server const &serv, Client &client) {
	struct stat st;

	try{
		if (stat(client.getPathToFile().c_str(), &st) != -1){
			cmpFileAndBody(client);
			if (client.getStatusCode() == 200)
				writeBodyInFile(client);
		}
		else{
			writeBodyInFile(client);
			client.setStatusCode(201);
		}
	}
	catch (std::exception &e){
		client.setStatusCode(500);
		return ;
	}

	setLastModified(client.getPathToFile());
	setContentLocation(client);
	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));
	_headers.insert(std::make_pair("Server", serv.get_serverName()));
	setContentLength("0");
	setDate();
	return;
}

void	Response::execGET(Server const &serv, Client &client){
	int		ret;
	char	buf[1024];
	int		file = open(client.getPathToFile().c_str(), O_RDONLY);
	
	if (file == -1){
		client.setStatusCode(500);
		return;
	}
	if (client.getMethod() == "HEAD")
		this->_headers.insert(std::make_pair("Content-Length", to_string(lseek(file, 0, SEEK_END))));
	else{
		while ((ret = read(file, buf, 1023)) > 0){
			buf[ret] = '\0';
			this->_body.append(buf);
		}
		if (ret == -1){
			close(file);
			client.setStatusCode(500);
			return ;
		}
		this->_headers.insert(std::make_pair("Content-Length", to_string(this->_body.size())));
	}
	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));
	setLastModified(client.getPathToFile());
	setContentType(client.getPathToFile());
	_headers.insert(std::make_pair("Content-Language", "en"));
	_headers.insert(std::make_pair("Server", serv.get_serverName()));
	setDate();
	close(file);
}

void Response::parseCgiFile() {
	int ret;
	int fd;
	char buf[10240];
	if ((fd = open("./cgiFile", O_RDWR, 0666)) == -1){
		this->setStatusCode(500);
		return;
	}
	std::string line("");
	while ((ret = read(fd, (void *)buf, (10240 - 1))) > 0){
		buf[ret] = '\0';
		line.append(buf);
	}
	if(ret == -1){
		close(fd);
		this->setStatusCode(500);
		return;
	}
	size_t poss;
	if ((poss = line.find("Status:")) != std::string::npos) {
		if(poss == 0) {
			this->setStatusCode(atoi(line.substr(8, 3).c_str()));
			if((poss = line.find("\r")) != std::string::npos)
				line.erase(0, poss + 2);
		}
	}
	while ((poss = line.find(':')) != std::string::npos) {
		std::pair<std::string, std::string> node;
		node.first = line.substr(0, poss);
		line.erase(0, poss + 2);
		if((poss = line.find("\r\n")) != std::string::npos) {
			node.second = line.substr(0, poss);
			line.erase(0, poss + 4);
			this->_headers.insert(node);
		}
	}
	this->_body.append(line);
	close(fd);
	unlink("./cgiFile");
}

void Response::execAfterCGI(Server const &serv, Client &client) {
	this->_headers.insert(std::make_pair("Content-Length", std::to_string(this->_body.size())));
	if (client.getMethod() == "HEAD")
		_body.clear();
	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));
	setLastModified(client.getPathToFile());
	setContentType(client.getPathToFile());
	_headers.insert(std::make_pair("Content-Language", "en"));
	_headers.insert(std::make_pair("Server", serv.get_serverName()));
	setDate();
}

void			Response::execListing(Server const &serv, Client &client){
	this->_body = getListing(client.getPathToFile(), serv, serv.get_locations()[client.getLocPos()]);
	this->_headers.insert(std::make_pair("Content-Length", to_string(this->_body.size())));
	if (client.getMethod() == "HEAD")
		_body.clear();
	if (this->_toClose == true)
		_headers.insert(std::make_pair("Connection", "close"));
	else
		_headers.insert(std::make_pair("Connection", "alive"));
	_headers.insert(std::make_pair("Content-Language", "en"));
	_headers.insert(std::make_pair("Server", serv.get_serverName()));
	setDate();
}

Response::Response(Server const &serv, Client &client):
		_version(serv.getEnvValue("SERVER_PROTOCOL")),
		_statusCode(client.getStatusCode()),
		_statusMessage(setStatusMessage(client.getStatusCode())),
		_body(""),
		_toClose(client.getToClose()) {
	if (client.getStatusCode() >= 400)
		setError(serv, client);
	else if (client.getStatusCode() == 243) {
		execListing(serv, client);
	}
	else{
		if (client.getWhere() == toCGI){
			parseCgiFile();
			execAfterCGI(serv, client);
		}
		else if (client.getMethod() == "GET" || client.getMethod() == "HEAD")
			execGET(serv, client);
		else if (client.getMethod() == "PUT")
			execPUT(serv, client);
		if (client.getStatusCode() >= 400)
			setError(serv, client);
	}
}

std::string		Response::createResponse(void) const{
	std::string		ret("");
	ret.append(this->_version);
	ret.append(" " + to_string(this->_statusCode));
	ret.append(" " + this->_statusMessage);
	ret.append("\r\n");
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	for (; it != _headers.end(); it++){
		ret.append(it->first + ": " + it->second + "\r\n");
	}
	ret.append("\r\n");
	if (!_body.empty()){
		ret.append(this->_body);
	}
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

void	Response::setContentLocation(Client &client) {
	size_t pos = 0;
	std::string uri = client.getRequest().getPath();
	if((pos = uri.rfind('?')) != std::string::npos)
		uri.erase(pos);
	if(uri.compare(0, 7, "http://") == 0)
		uri.erase(0, 7);
	this->_headers.insert(std::make_pair("Content-Location", uri));
}

void	Response::setContentType(std::string const &pathToFile){
	std::string file(pathToFile.substr(pathToFile.rfind('/'), pathToFile.size()));
	size_t pos;
	if ((pos = file.rfind('.')) == std::string::npos)
		this->_headers.insert(std::make_pair("Content-Type", "text/plain"));
	else{
		std::string extension(file.substr(pos, file.size()));
		this->_headers.insert(std::make_pair("Content-Type", findMimeType(extension)));
	}
}

void	Response::setContentLength(std::string const &length) {
	this->_headers.insert((std::make_pair("Content-Length", length)));
}

void	Response::setLastModified(std::string const &file) {
	struct tm		timeinfo;
	char			buffer[30];
	struct stat		st;

	stat(file.c_str(), &st);
	std::string time(to_string(st.st_mtimespec.tv_sec));
	strptime(time.c_str(), "%s", &timeinfo);
	strftime(buffer,30,"%a, %d %b %G %T %Z", &timeinfo);
	this->_headers.insert(std::make_pair("Last-Modified", (std::string)(buffer)));
}

void	Response::setDate(void){
	char			buffer[30];
	struct tm		timeinfo;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	std::string time(to_string(tv.tv_sec));
	strptime(time.c_str(), "%s", &timeinfo);
	strftime(buffer, 30,"%a, %d %b %G %T %Z", &timeinfo);
	this->_headers.insert(std::make_pair("Date", (std::string)(buffer)));
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
