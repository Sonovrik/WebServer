//
// Created by Kory Moaning on 3/20/21.
//

#include "RequestConfigMatch.hpp"

std::vector<std::string> splitString(std::string str) {
	std::vector<std::string> ret;
	size_t pos;
	while ((pos = str.find(' ')) != std::string::npos) {
		ret.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	ret.push_back(str);
	return ret;
}

int compareLocation(std::string &uri, location_t loc, std::string &res) {
	std::string name = loc._name;
//	std::string root = loc._directives.find("root")->second;
//	int len = rootLen ? name.length() : name.length() + root.length();
	int len = name.length();
	if(uri.compare(0, len, name) == 0) {
		res = name;
		uri.erase(0, len);
		return 1;
	}
	return 0;
}

std::string getLocation(std::string &uri, Server &ser, int &pos) {
	std::string res;
	std::vector<location_t> tmp = ser.get_locations();
	size_t count = tmp.size();
	size_t i = 0;
	while (i < count) {
		if (compareLocation(uri, tmp[i], res))
			break;
		if (compareLocation(uri, tmp[i], res))
			break;
		i++;
	}
//	if(i == count)
//		res = ser.get_root() + res;
	pos = i;
	return res;
}

int checkIndex(std::string &ret,const Server &ser, std::string &location) {

}

std::string getPath(std::string &uri, int &loc, Request &req, const Server &ser) {
//	std::string tmp = location + uri;
	std::string tmp = ser.get_root() + '/';
	if (!ser.get_locations()[loc]._directives.find("root")->second.empty())
		tmp = tmp + ser.get_locations()[loc]._directives.find("root")->second + '/' + uri;
	size_t len = tmp.length();
	size_t count = 0;
	struct stat info;
	for (size_t i = 0; i < len; ++i) {
		if (tmp[i] == '/')
			count++;
	}
	size_t pos = tmp.find('/');
	if(pos == std::string::npos)
		; //error?? не будет
	std::string ret = tmp.substr(0, pos + 1);
	tmp.erase(0, pos + 1);
	const char *path = ret.c_str();
	for (size_t i = 0; i <= count; ++i) {
		if (lstat(path, &info) == 0){
			if ((pos = tmp.find('/')) == std::string::npos) {
				if(tmp.length())
					pos = tmp.length() - 1;
			}
			ret += tmp.substr(0, pos + 1);
			tmp.erase(0, pos + 1);
			path = ret.c_str();
			continue;
		}
		break;
	}
	size_t last = ret.length() - 1;
	if (ret[last] == '/' && tmp.length() == 0) { // папка, checkIndex
//		path = checkIndex(ret, ser, location);
		if (stat(path, &info) != 0)
			throw std::runtime_error("paht not found. code: 404 Not Found");
	}
	else if(ret[last] == '/' && tmp.length() != 0) { // точно файл, проверить его наличие
		ret.erase(ret.length() - 1, 1);
		path = ret.c_str();
		req.setPathInfo(tmp);
		if (stat(path, &info) != 0)
			throw std::runtime_error("paht not found. code: 404 Not Found");
	}
	return (ret);
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
		if (req.getBody().size() > len)
			throw std::runtime_error("Payload Too Large. code: 413 ");
	}
}

void setWhere(Server &ser, int locIndex, Request &req, Client &client) {
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
		if (extension == extConf && cgiPath == cgiPathConf) {
			client.setWhere(1) ;//where = cgi;
//			if(req.getPathInfo().empty())
//
		}
		else
			client.setWhere(0) ; //where != cgi; какое то поле не совпало
	}
	else {
		// ес..ath, то??
	}
}

void checkConf(Server &ser, int locIndex, Request &req, Client &client) {
	if(ser.get_locations()[locIndex]._directives.find("method") != ser.get_locations()[locIndex]._directives.end()) {
		std::string method = ser.get_locations()[locIndex]._directives.find("method")->second;
		if (!method.empty()) {
			std::vector<std::string> methods = splitString(method);
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
	setWhere(ser, locIndex, req, client);
}

int RequestConfigMatch(Client &client, Server &ser) {
	Request &req = client.getRequest();
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
		req.setPathInfo("cgi_tester"); // проверить у Маши проставление /CGI/cgi_tester
		if((pos = uri.find('/')) != std::string::npos) {
			checkHost(pos, uri, ser);
		}
//			else
//				; //error
//		}
		location = getLocation(uri, ser, loc);
		pathToScript = getPath(uri, loc, req, ser);
		req.setPath(pathToScript); //  переделать по нормальному!!!
		checkConf(ser, loc, req, client);
		client.setPathToFile(pathToScript);
	}
	catch (std::exception &exception) {
		std::cerr << "error : " << exception.what() << std::endl;
		return -1;
	}
	std::cout << "pathToScript : " << pathToScript << std::endl << "path Info : " << req.getPathInfo() << std::endl;
	return 0;
}