//
// Created by Kory Moaning on 3/2/21.
//

#include "CGI.hpp"
#include "../utils.hpp"

int comparServName(std::string &uri, location_t tmp, std::string &res, bool rootLen) {
	std::string root = tmp._directives.find("root")->second;
	std::string name = tmp._name;
	int len = rootLen ? name.length() : name.length() + root.length();
	if(uri.compare(0, len, name) == 0) {
		res = root + name;
		uri.erase(0, len);
		return 1;
	}
	return 0;
}

std::string getLocation(std::string &uri, Server &ser, int *pos) {
	std::string res;
	std::vector<location_t> tmp = ser.get_locations();
	size_t count = tmp.size();
	int i = 0;
	while (i < count) {
		if (comparServName(uri, tmp[i], res, false))
			break;
		if (comparServName(uri, tmp[i], res, true))
			break;
		i++;
	}
	if(i == count)
		res = ser.get_root() + "/";
	*pos = i;
	return res;
}

std::string getPath(std::string &uri, std::string &location, Request &req) {
	std::string res = location + uri;
	size_t len = res.length();
	size_t count = 0;
	struct stat info;
	for (int i = 0; i < len; ++i) {
		if (uri[i] == '/')
			count++;
	}
	int pos = res.find('/');
	if(pos == std::string::npos)
		; //error??
	std::string tmp = res.substr(0, pos + 1);
	res.erase(0, pos + 1);
	const char *path = tmp.c_str();
	for (int i = 0; i <= count; ++i) {
		if (stat(path, &info) == 0){
			if ((pos = res.find('/')) == std::string::npos) {
				if(res.length())
				pos = res.length() - 1;
			}
				tmp += res.substr(0, pos + 1);
				res.erase(0, pos + 1);
				path = tmp.c_str();
				continue;
		}
		break;
	}
	size_t last = tmp.length() - 1;
	if(tmp[last] == '/' && res.length() != 0) {  // что если придет запрос  http://localhost:8080/put_test/1.php/
		tmp.erase(tmp.length() - 1, 1);
		path = tmp.c_str();
		req.setPathInfo(res);
		if (stat(path, &info) != 0)
			throw "paht not found. code: 404 Not Found";
	}
	return (tmp);
}

std::vector<std::string> splitMethods(std::string method) {
	std::vector<std::string> ret;
	int pos;
	while ((pos = method.find(' ')) != std::string::npos) {
		ret.push_back(method.substr(0, pos));
		method.erase(0, pos + 1);
	}
	ret.push_back(method);
	return ret;
}

void checkBodySize(Server &ser, int pos, Request &req) {
	std::string maxBody;
	if(ser.get_locations()[pos]._directives.find("max_body_size") != ser.get_locations()[pos]._directives.end()) // может ли быть поле пустым?
		maxBody = ser.get_locations()[pos]._directives.find("max_body_size")->second;
	else if (!ser.get_maxBodySize().empty())
		maxBody = ser.get_maxBodySize();
	else
		maxBody = "";
	if (!maxBody.empty()) {
		double len = strtod(maxBody.c_str(), NULL);
		if (req.getBody().size() > len)  // если пришло чанками то что?
			throw "Payload Too Large. code: 413 ";
	}
}

void checkConf(Server &ser, int pos, Request &req) {
	if(ser.get_locations()[pos]._directives.find("method") != ser.get_locations()[pos]._directives.end()) {
		std::string method = ser.get_locations()[pos]._directives.find("method")->second;
		if (!method.empty()) {
			std::vector<std::string> methods = splitMethods(method);
			size_t size = methods.size();
			size_t i;
			for (i = 0; i < size; ++i) {
				if(methods[i] == req.getMethod())
					break;
			}
			if(i == size)
				throw "Method Not Allowed. code: 405????"; // не может быть при GET/HEAD
		}
	}
	checkBodySize(ser, pos, req);
}

int parsBeforeCGI(Request &req, Server &ser) {
	std::string uri = req.getPath();
	std::string host;
	std::string hostName;
	std::string pathToScript;
	std::string port;
	std::string location;
	if(uri.compare(0, 7, "http://") == 0)
		uri.erase(0, 7); //delete http:// разобраться!!!!
	int pos;
	if((pos = uri.rfind('?')) != std::string::npos)
		uri.erase(pos);
	pos = 0;
	if((pos = uri.find('/')) != std::string::npos) {
		if(pos != 1) {  // как понять что это не 1.php/user/bin/php?q=r&a=d
			host.assign(uri, 0, pos);
			if(host != (ser.get_serverName() + ":" + ser.get_port()) && host != (ser.get_ip() + ":" + ser.get_port())) {
				int i;
				if((i = host.find(':')) != std::string::npos) {
					hostName.assign(host, 0, i);
					port = host.substr(i + 1);
					if(hostName != ser.get_ip() && hostName != ser.get_serverName()) {
						std::cerr << "error! Server name does not match configuration" << std::endl; // код ошибки??
						return -1;
					}
					else if(port != ser.get_port()) {
						std::cerr << "error! Port does not match configuration" << std::endl; // код ошибки??
						return -1;
					}
				}
				else
					hostName = host;
				uri.erase(0, host.length());
			}
			else {
				int i;
				if((i = host.find(':')) != std::string::npos) {
					hostName.assign(host, 0, i);
					port = host.substr(i + 1);
				}
				else
					hostName = host;
				uri.erase(0, host.length());
			}
		}
	}
	else
		;// не нашел '/' вообще, может ли быть GET 1.php
	int loc = -1;
	try{
		location = getLocation(uri, ser, &loc);
		pathToScript = getPath(uri, location, req);
		req.setPath(pathToScript); //  переделать по нормальному!!!
		checkConf(ser, loc, req);
	}
	catch (const char *str){
		std::cerr << "error : " << str << std::endl;
		return -1;
	}
//	std::cout << "uri : " << uri << std::endl << "path : " << req.getPath() << std::endl;
//	std::cout << "host : " << host << std::endl << "hostName : " << hostName << std::endl;
//	std::cout << "port : " << port << std::endl  << "location : " << location << std::endl;
//	std::cout << "pathToScript : " << req.getPath() << std::endl << "path Info : " << req.getPathInfo() << std::endl;
	return 0;
}


int main(){

	ConfigParser parser;
	parser.parseConfig("webserv.conf");
	std::vector<Server> _serversList(parser.getServers());
	for (std::vector<Server>::iterator it = _serversList.begin(); it < _serversList.end(); it++){
		it->fullConfigEnvironment();
	}
	Request req;
	std::string tmp3 = "POST http://127.0.0.1:8081/html/YoupiBanane/1.bla/CGI/cgi_tester HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nAuthorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l\r\n\r\nbulina,masha,den!\r\n";
//	std::string tmp3 = "PUT http://localhost:8080/post_body/1.php/user/bin/php?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
	parseRequest(tmp3, req);
	if(parsBeforeCGI(req, _serversList.front()) == 0) {
		CGI qqq(req, _serversList.front());
		qqq.init(req, _serversList.front());
		qqq.creatENV();
		qqq.exec();
	}
	return 0;
}
