//
// Created by Kory Moaning on 3/2/21.
//

#include "CGI.hpp"
#include "../utils.hpp"

int compareLocation(std::string &uri, location_t loc, std::string &res, bool rootLen) {
	std::string name = loc._name;
	std::string root = loc._directives.find("root")->second;
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
	size_t i = 0;
	while (i < count) {
		if (compareLocation(uri, tmp[i], res, false))
			break;
		if (compareLocation(uri, tmp[i], res, true))
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
	for (size_t i = 0; i < len; ++i) {
		if (uri[i] == '/')
			count++;
	}
	size_t pos = res.find('/');
	if(pos == std::string::npos)
		; //error??
	std::string tmp = res.substr(0, pos + 1);
	res.erase(0, pos + 1);
	const char *path = tmp.c_str();
	for (size_t i = 0; i <= count; ++i) {
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
	if(tmp[last] == '/' && res.length() != 0) {
		tmp.erase(tmp.length() - 1, 1);
		path = tmp.c_str();
		req.setPathInfo(res);
		if (stat(path, &info) != 0)
			throw std::runtime_error("paht not found. code: 404 Not Found");
	}
	return (tmp);
}

void compareHostName(const std::string& hostName, const std::string& ip, const std::string& servName) {
	if(hostName != ip && hostName != servName)
		throw std::runtime_error("error! Server name does not match configuration! code: ????"); // код ошибки??
}

void comparePort(const std::string& port, const std::string& servPort) {
	if(port != servPort)
		throw std::runtime_error("error! Port does not match configuration! code: ????"); // код ошибки??
}

int checkHost(int pos, std::string &uri, Server &ser) {
	std::string port;
	std::string host;
	std::string hostName;
	size_t colon;
	if(pos != 1) {
		host.assign(uri, 0, pos);
		if(host != (ser.get_serverName() + ":" + ser.get_port()) && host != (ser.get_ip() + ":" + ser.get_port())) {
			if((colon = host.find(':')) != std::string::npos) {
				hostName.assign(host, 0, colon);
				port = host.substr(colon + 1);
				compareHostName(hostName, ser.get_ip(), ser.get_serverName());
				comparePort(port, ser.get_port());
			}
			else
				hostName = host; // error?
			uri.erase(0, host.length());
		}
		else {
			colon = host.find(':');
			hostName.assign(host, 0, colon);
			port = host.substr(colon + 1);
			uri.erase(0, host.length());
		}
	}
	return 0;
}

std::vector<std::string> splitMethods(std::string method) {
	std::vector<std::string> ret;
	size_t pos;
	while ((pos = method.find(' ')) != std::string::npos) {
		ret.push_back(method.substr(0, pos));
		method.erase(0, pos + 1);
	}
	ret.push_back(method);
	return ret;
}

void checkBodySize(Server &ser, int locIndex, Request &req) {
	std::string maxBody;
	std::map<std::string, std::string> dir = ser.get_locations()[locIndex]._directives;
	if(dir.find("max_body_size") != dir.end()) // может ли быть поле пустым?
		maxBody = ser.get_locations()[locIndex]._directives.find("max_body_size")->second;
	else if (!ser.get_maxBodySize().empty())
		maxBody = ser.get_maxBodySize();
	else
		maxBody = "";
	if (!maxBody.empty()) {
		double len = strtod(maxBody.c_str(), NULL);
		if (req.getBody().size() > len)  // если пришло чанками то что?
			throw std::runtime_error("Payload Too Large. code: 413 ");
	}
}

void setWhere(Server &ser, int locIndex, Request &req) {
	std::string extension;
	std::string cgiPath;
	std::map<std::string, std::string> dir = ser.get_locations()[locIndex]._directives;
	std::string extConf = dir.find("cgi_extensions") != dir.end() ? dir.find("cgi_extensions")->second : ""; //может быть несколько??
	std::string cgiPathConf = dir.find("cgi_path") != dir.end() ? dir.find("cgi_path")->second : "";
	const std::string& tmp = req.getPath();
	size_t pos;
	if((pos = tmp.find('.')) != std::string::npos) {
		extension = tmp.substr(pos);
	}
	if (!cgiPathConf.empty() || !extConf.empty()) {
		if (extension == extConf && cgiPath == cgiPathConf)
			;//where = cgi;
		else
			; //where = cgi; какое то поле не совпало,
	}
	else {
		// если в локейшене конфига не заданы поля cgi_extensions и cgi_path, то??
	}
}

void checkConf(Server &ser, int locIndex, Request &req) {
	if(ser.get_locations()[locIndex]._directives.find("method") != ser.get_locations()[locIndex]._directives.end()) {
		std::string method = ser.get_locations()[locIndex]._directives.find("method")->second;
		if (!method.empty()) {
			std::vector<std::string> methods = splitMethods(method);
			size_t size = methods.size();
			size_t i;
			for (i = 0; i < size; ++i) {
				if(methods[i] == req.getMethod())
					break;
			}
			if(i == size)
				throw std::runtime_error("Method Not Allowed. code: 405????"); // не может быть при GET/HEAD
		}
	}
	checkBodySize(ser, locIndex, req);
	setWhere(ser, locIndex, req);
}

int parsBeforeCGI(Request &req, Server &ser) {
	std::string uri = req.getPath();
	std::string pathToScript;
	std::string location;
	int loc = -1;
	size_t pos;
	if((pos = uri.rfind('?')) != std::string::npos)
		uri.erase(pos);
	pos = 0;
	try {  // Сказать Маше оставлять http
//		if(uri.compare(0, 7, "http://") == 0) {
//			uri.erase(0, 7); //delete http:
			if((pos = uri.find('/')) != std::string::npos) {
				checkHost(pos, uri, ser);
			}
//			else
//				; //error
//		}
		location = getLocation(uri, ser, &loc);
		pathToScript = getPath(uri, location, req);
		req.setPath(pathToScript); //  переделать по нормальному!!!
		checkConf(ser, loc, req);
	}
	catch (std::exception &exception) {
		std::cerr << "error : " << exception.what() << std::endl;
		return -1;
	}
	std::cout << "pathToScript : " << req.getPath() << std::endl << "path Info : " << req.getPathInfo() << std::endl;
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
	std::string tmp3 = "POST http://localhost:8081/html/YoupiBanane/1.bla/CGI/cgi_tester?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nAuthorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l\r\n\r\nhi, mahmud\r\n";
//	std::string tmp3 = "PUT http://localhost:8080/post_body/1.php/user/bin/php?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
	parseRequest(tmp3, req);
	if(parsBeforeCGI(req, _serversList.front()) == 0) {
		try {
			CGI qqq(req, _serversList.front());
			qqq.init(req, _serversList.front());
			qqq.creatENV();
			qqq.exec();
		}
		catch (std::exception &exception) {
			std::cerr << "error : " << exception.what() << std::endl;
			return 1;
		}
	}
	return 0;
}
