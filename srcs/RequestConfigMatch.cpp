//
// Created by Kory Moaning on 3/20/21.
//

#include "RequestConfigMatch.hpp"

void	setErrorCode(const std::string& str, Client &client) {
	client.setStatusCode( atoi(str.c_str()));
}

std::vector<std::string>	splitString(std::string &str) {
	std::vector<std::string>	ret;
	size_t						pos;

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
		if (str[i] == c)
			count++;
	}
	return count;
}

std::string			b64decode(const void* data, const size_t len)
{
	unsigned char* p = (unsigned char*)data;
	int pad = len > 0 && (len % 4 || p[len - 1] == '=');
	const size_t L = ((len + 3) / 4 - pad) * 4;
	std::string str(L / 4 * 3 + pad, '\0');

	for (size_t i = 0, j = 0; i < L; i += 4) {
		int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
		str[j++] = n >> 16;
		str[j++] = n >> 8 & 0xFF;
		str[j++] = n & 0xFF;
	}
	if (pad) {
		int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
		str[str.size() - 1] = n >> 16;
		if (len > L + 2 && p[L + 2] != '=') {
			n |= B64index[p[L + 2]] << 6;
			str.push_back(n >> 8 & 0xFF);
		}
	}
	return str;
}

int	compareLocation(std::string &uri, const location_t &loc) {
	std::string name = loc._name;
	size_t len = name.length();

	if (len > 1 && name[len - 1] == '/')
		len -= 1;
	if(name.compare(0, len, uri.substr(0,len)) == 0) {
		if (len > 1)
			uri.erase(0, len);
		return 1;
	}
	else if (std::regex_match(uri, std::regex(name))) {
		if (len > 1)
			uri.erase(0, len);
		return 1;
	}
	return 0;
}

void	getLocation(std::string &uri, Server &ser, int &pos) {
	size_t count = ser.get_locations().size();
	int i = 0;

	while (i < count) {
		if (compareLocation(uri, ser.get_locations()[i]))
			break;
		i++;
	}
	if (i == count)
		throw std::runtime_error("404");
	pos = i;
}

int	autoIndex(std::string &ret, Server const &serv, const location_t &location) {
	if (location._directives.find("autoindex")->second == "on")
		return 1;
	else
		throw std::runtime_error("404");		//"Index Not Found, code: 403");
}

int	checkIndex(std::string &ret, Server const &serv, const location_t &location) {
	struct stat info;
	std::string tmp;
	size_t i;

	if(location._directives.find("index") != location._directives.end()) {
		std::string str = location._directives.find("index")->second;
		if (!str.empty()) {
			std::vector<std::string> index = splitString(str);
			size_t size = index.size();
			for (i = 0; i < size; ++i) {
				tmp = ret + index[i];
				if(stat(tmp.c_str() , &info) == 0)
					break;
			}
			if(i != size) {
				ret = tmp;
				return 0;
			}
		}
	}
	return (autoIndex(ret, serv, location));
}

std::string	getPath(std::string &uri, int &loc, Request &req, const Server &ser) {
	struct stat info;
	std::string tmp;

	tmp = ser.get_locations()[loc]._directives.find("root")->second + uri;
	size_t count = countChar(tmp, '/');
	size_t pos = tmp.find('/');
	std::string ret = tmp.substr(0, pos + 1);
	tmp.erase(0, pos + 1);
	const char *path = ret.c_str();
	for (size_t i = 0; i <= count; ++i) {
		if (stat(path, &info) == 0) {
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
	if(tmp.length() != 0) {
		if (ret[ret.length() - 1] == '/')
			ret.erase(ret.length() - 1, 1);
		path = ret.c_str();
		req.setPathInfo(tmp);
		if (stat(path, &info) != 0)
			throw std::runtime_error("404");
	} else {
		path = ret.c_str();
		if (stat(path, &info) != 0) {
			if (req.getMethod() != "PUT" && req.getMethod() != "POST")
				throw std::runtime_error("404");
		}
		else {
			if (S_ISDIR(info.st_mode)) {
				ret += ret.back() != '/' ? "/" : "";
				if (checkIndex(ret, ser, ser.get_locations()[loc])) {
					req.setPath(ret);
					throw std::runtime_error("243");
				}
			}
		}
	}
	req.setPath(ret);
	return (ret);
}

void	compareHostName(const std::string& hostName, const std::string& ip, const std::string& servName) {
	if(hostName != ip && hostName != servName)
		throw std::runtime_error("400");
}

void	comparePort(const std::string& port, const std::string& servPort) {
	if(port != servPort)
		throw std::runtime_error("400");
}

int	checkHost(size_t &pos, std::string &uri, Server &ser) {
	std::string host;
	std::string hostName;
	size_t colon;

	host.assign(uri, 0, pos);
	if(host != ser.get_serverName() && host != (ser.get_serverName() + ":" + ser.get_port())
	&& host != (ser.get_ip() + ":" + ser.get_port())) {
		if((colon = host.find(':')) != std::string::npos) {
			hostName.assign(host, 0, colon);
			compareHostName(hostName, ser.get_ip(), ser.get_serverName());
			comparePort(host.substr(colon + 1), ser.get_port());
		}
		uri.erase(0, host.length());
	}
	return 0;
}

void	checkBodySize(Server &ser, int &locIndex, Request &req) {
	std::string maxBody = ser.get_locations()[locIndex]._directives.find("max_body_size")->second;
	if (!maxBody.empty()) {
		size_t len = atoi(maxBody.c_str());
		if (req.getBody().size() > len)
			throw std::runtime_error("413");
	}
}

bool	getWhere(std::map<std::string, std::string> const &dir, Request &req) {
	std::string	extReq;
	size_t		pos;

	if((pos = req.getPath().rfind('.')) != std::string::npos)
		extReq = req.getPath().substr(pos);
	if (dir.find("cgi_extensions")->second.find(extReq) == std::string::npos)
		return false;
	if (req.getPathInfo().empty())
		req.setPathInfo(dir.find("cgi_path")->second);
	if(req.getPathInfo() != dir.find("cgi_path")->second)
		return false;
	const std::string& method = req.getMethod();
	if(method == "GET" || method == "HEAD" || method == "POST")
		return true;
	return false;
}
bool passUser(const std::string& user, const std::string& pas, const std::string &path) {
	std::string	file;
	std::string	line;
	int			fd;
	char		buf[10240];
	ssize_t		pos;

	if ((fd = open(path.c_str(), O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG)) == -1)
		throw std::runtime_error("500");
	while ((pos = read(fd, (void *)buf, (10240 - 1))) > 0) {
		buf[pos] = '\0';
		file.append(buf);
	}
	if(pos == -1)
		throw std::runtime_error("500");
	while (!file.empty()) {
		if ((pos = file.find('\n')) != std::string::npos) {
			line = file.substr(0, pos);
			file.erase(0, pos + 1);
		}
		else {
			line = file;
			file.erase(0);
		}
		trimString(line);
		if ((pos = line.find(':')) != std::string::npos) {
			if (line.substr(0, pos) == user && line.substr(pos + 1) == pas) {
				close(fd);
				return true;
			}
		}
	}
	close(fd);
	return false;
}

bool checkAutorization(Request &req, int &locIndex, Server &ser) {
	std::map<std::string, std::string> map =  ser.get_locations()[locIndex]._directives;
	if(map.find("auth_basic") != map.end()) {
		if(req.getHeaders().find("AUTHORIZATION") == req.getHeaders().end() ||
		req.getHeaders().find("AUTHORIZATION")->second.empty())
			return false;
		size_t pos = req.getHeaders().find("AUTHORIZATION")->second.rfind(' ');
		if(pos == std::string::npos)
			return false;
		if(req.getHeaders().find("AUTHORIZATION")->second.substr(0, pos) != "Basic")
			return false;
		std::string tmpFoIdent = req.getHeaders().find("AUTHORIZATION")->second.substr(pos + 1);
		tmpFoIdent = b64decode(tmpFoIdent.c_str(), tmpFoIdent.size());
		if((pos = tmpFoIdent.find(':')) == std::string::npos)
			return false;
		if(map.find("auth_basic_user_file") != map.end()) {
			return passUser(tmpFoIdent.substr(0, pos), tmpFoIdent.substr(pos + 1),
			map.find("auth_basic_user_file")->second);
		}
		else
			throw std::runtime_error("500");
	}
	return true;
}

int checkMethods(Server &ser, int &locIndex, Request &req) {
	if(ser.get_locations()[locIndex]._directives.find("method") != ser.get_locations()[locIndex]._directives.end()) {
		if(ser.get_locations()[locIndex]._directives.find("method")->second.find(req.getMethod()) == std::string::npos)
			return 0;
	}
	return 1;
}

void	checkConf(Server &ser, int &locIndex, Request &req, Client &client) {
	if(!checkMethods(ser, locIndex, req))
			throw std::runtime_error("405");
	checkBodySize(ser, locIndex, req);
	if (!checkAutorization(req, locIndex, ser))
		throw std::runtime_error("401");
	if (getWhere(ser.get_locations()[locIndex]._directives, req))
		client.setWhere(toCGI);
	else {
		if (req.getMethod() == "POST")
			throw std::runtime_error("500");
		client.setWhere(toServer);
	}
}

int RequestConfigMatch(Client &client, Server &ser) {
	Request &req = client.getRequest();
	std::string uri = req.getPath();
	int loc = 0;
	size_t pos;

	try {
		if((pos = uri.rfind('?')) != std::string::npos)
			uri.erase(pos);
		pos = 0;
		if(uri.compare(0, 7, "http://") == 0) {
			uri.erase(0, 7);
			if((pos = uri.find('/')) != std::string::npos)
				checkHost(pos, uri, ser);
		}
		getLocation(uri, ser, loc);
		client.setLocPos(loc);
		client.setPathToFile(getPath(uri, loc, req, ser));
		checkConf(ser, loc, req, client);
	}
	catch (std::exception &exception) {
		client.setPathToFile(req.getPath());
		setErrorCode(exception.what(), client);
		if(client.getStatusCode() == 243) {
			if (!checkAutorization(req, loc, ser))
				client.setStatusCode( 401);
			if (!checkMethods(ser, loc, req))
				client.setStatusCode( 405);
		}
		return -1;
	}
	return client.getWhere();
}
