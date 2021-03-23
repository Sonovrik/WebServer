//
// Created by Kory Moaning on 3/20/21.
//

#include "RequestConfigMatch.hpp"

void	setErrorCode(const std::string& str, Client &client) {
	client.setStatusCode( atoi(str.c_str()));
	std::cerr << "error : " << str << std::endl;
}

std::vector<std::string>	splitString(std::string str) {
	std::vector<std::string> ret;
	size_t pos;
	while ((pos = str.find(' ')) != std::string::npos) {
		ret.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	ret.push_back(str);
	return ret;
}

size_t	countChar(const std::string& str, char c) {
	size_t len = str.length();
	size_t count = 0;
	for (size_t i = 0; i < len; ++i) {
		if (str[i] == '/')
			count++;
	}
	return count;
}

int	compareLocation(std::string &uri, location_t loc, std::string &res) {
	std::string name = loc._name;
	int len = name.length();
	if(uri.compare(0, len, name) == 0) {
		res = name;
		uri.erase(0, len);
		return 1;
	}
	return 0;
}

std::string	getLocation(std::string &uri, Server &ser, int &pos) {
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
	if (i == count)
		throw std::runtime_error("404");
	pos = i;
	return res;
}

void	checkIndex(std::string &ret, location_t &location) {
	struct stat info;
	std::string tmp;
	std::string str = location._directives.find("index")->second;
	if (!str.empty()) {
		std::vector<std::string> index = splitString(str);
		size_t size = index.size();
		size_t i;
		for (i = 0; i < size; ++i) {
			tmp = ret + index[i];
			if(stat(tmp.c_str() , &info) == 0)
				break;
		}
		if(i == size) {									// не нашла index
			if (location._directives.find("autoindex")->second == "on")
				;										// листинг директорий
			else
				throw std::runtime_error("403");		//"Index Not Found, code: 403");
		}
		else											// нашла индекс, добавила его к директории
			ret = tmp;
	}
}

std::string	getPath(std::string &uri, int &loc, Request &req, const Server &ser) {
	struct stat info;
	std::string tmp = ser.get_root() + '/';
	if (!ser.get_locations()[loc]._directives.find("root")->second.empty())
		tmp = tmp + ser.get_locations()[loc]._directives.find("root")->second + '/' + uri;
	size_t count = countChar(tmp, '/');
	size_t pos = tmp.find('/');
	if(pos == std::string::npos)
		; //error?? не будет
	std::string ret = tmp.substr(0, pos + 1);
	tmp.erase(0, pos + 1);
	const char *path = ret.c_str();
	for (size_t i = 0; i <= count; ++i) {
		if (stat(path, &info) == 0){
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
	if (ret[last] == '/' && tmp.length() == 0) {			//папка, checkIndex
		checkIndex(ret, ser.get_locations()[loc]);
	}
	else if(ret[last] == '/' && tmp.length() != 0) {		//точно файл, проверить его наличие
		ret.erase(ret.length() - 1, 1);
		path = ret.c_str();
		req.setPathInfo(tmp);
		if (stat(path, &info) != 0)
			throw std::runtime_error("404");				//"paht not found. code: 404 Not Found";
	}
	else{
		// ret.erase(ret.length() - 1, 1);
		path = ret.c_str();
		// req.setPathInfo(tmp);
		if (stat(path, &info) != 0)
			throw std::runtime_error("404");
	}
		; // проверить если файл без пути интерпритатора
	return (ret);
}

void	compareHostName(const std::string& hostName, const std::string& ip, const std::string& servName) {
	if(hostName != ip && hostName != servName)
		throw std::runtime_error("400"); //"error! Server name does not match configuration! code: ????";
}

void	comparePort(const std::string& port, const std::string& servPort) {
	if(port != servPort)
		throw std::runtime_error("400"); //"error! Port does not match configuration! code: ????";
}

int	checkHost(int pos, std::string &uri, Server &ser) {
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
				compareHostName(host, ser.get_ip(), ser.get_serverName());
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

void	checkBodySize(Server &ser, int locIndex, Request &req) {
	std::string maxBody;
	maxBody = ser.get_locations()[locIndex]._directives.find("max_body_size")->second;
	if (!maxBody.empty()) {
		double len = strtod(maxBody.c_str(), NULL);
		if (req.getBody().size() > len)
			throw std::runtime_error("413"); // "Payload Too Large. code: 413 "
	}
}

bool	getWhere(std::map<std::string, std::string> dir, Request &req) {
	std::string extReq;
	const std::string& pathReq = req.getPathInfo();
	std::string extsConf = dir.find("cgi_extensions")->second;
	std::string pathConf = dir.find("cgi_path")->second;
	std::vector<std::string> extConfArr = splitString(extsConf);
	size_t extCount = extConfArr.size();
	const std::string& tmp = req.getPath();
	size_t pos;
	if((pos = tmp.rfind('.')) != std::string::npos)
		extReq = tmp.substr(pos);					//вместе с точкой
	else
		extReq = "";
	size_t i;
	for (i = 0; i < extCount; ++i) {
		if (extReq == extConfArr[i])
			break;
	}
	if (i == extCount)
		return false;
	if(pathReq != pathConf)
		return false;
	const std::string& method = req.getMethod();
	if(method == "GET" || method == "HEAD" || method == "POST")
		return true; // true == cgi == 1 in Where
	return false;
}

void	checkConf(Server &ser, int locIndex, Request &req, Client &client) {
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
				throw std::runtime_error("405"); // не может быть при GET/HEAD
		}
	}
	checkBodySize(ser, locIndex, req);
	getWhere(ser.get_locations()[locIndex]._directives, req) == true ? client.setWhere(toCGI) : client.setWhere(toServer);
}

int RequestConfigMatch(Client &client, Server &ser) {
	Request &req = client.getRequest();
	std::string uri = req.getPath();
	std::string pathToScript;
	int loc = -1;
	size_t pos;
	if((pos = uri.rfind('?')) != std::string::npos)
		uri.erase(pos);
	pos = 0;
	req.setPathInfo("/Users/kmoaning/Desktop/ToGit/cgi_tester"); //  /Users/kmoaning/.brew/bin/php-cgi
	try {
		if(uri.compare(0, 7, "http://") == 0) {
			uri.erase(0, 7); //delete http:
			if((pos = uri.find('/')) != std::string::npos)
				checkHost(pos, uri, ser);
		}
		else
				; //сразу путь без схемы и хоста
		getLocation(uri, ser, loc);
		pathToScript = getPath(uri, loc, req, ser);
		req.setPath(pathToScript);
		client.setPathToFile(pathToScript);
		checkConf(ser, loc, req, client);
	}
	catch (std::exception &exception) {
		setErrorCode(exception.what(), client);
		return -1;
	}
//	std::cout << "pathToScript : " << pathToScript << std::endl << "path Info : " << req.getPathInfo() << std::endl;
	return client.getWhere();
}