//
// Created by Kory Moaning on 3/2/21.
//

#include "CGI.hpp"
#include "../utils.hpp"
#include "../Response.hpp"
#include "../Client/Client.hpp"

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

//int checkIndex(std::string &ret,const Server &ser, std::string &location) {
//
//}

std::string getPath(std::string &uri, std::string &location, Request &req, const Server &ser) {
	std::string tmp = location + uri;
	size_t len = tmp.length();
	size_t count = 0;
	struct stat info;
	for (size_t i = 0; i < len; ++i) {
		if (uri[i] == '/')
			count++;
	}
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
		if (req.getBody().size() > len)
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
			; //where != cgi; какое то поле не совпало
	}
	else {
		// ес..ath, то??
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

int parsBeforeCGI(Client &client, Server &ser) {
	Request req = client.getRequest();
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
		pathToScript = getPath(uri, location, req, ser);
		req.setPath(pathToScript); //  переделать по нормальному!!!
		checkConf(ser, loc, req);
	}
	catch (std::exception &exception) {
		std::cerr << "error : " << exception.what() << std::endl;
		return -1;
	}
	std::cout << "pathToScript : " << pathToScript << std::endl << "path Info : " << req.getPathInfo() << std::endl;
	return 0;
}

//int main(){
//
//	ConfigParser parser;
//	parser.parseConfig("webserv.conf");
//	std::vector<Server> _serversList(parser.getServers());
//	for (std::vector<Server>::iterator it = _serversList.begin(); it < _serversList.end(); it++){
//		it->fullConfigEnvironment();
//	}
//	Request req;
//	std::string tmp3 = "POST http://localhost:8081/html/YoupiBanane/1.bla/CGI/cgi_tester?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nAuthorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l\r\n\r\ngbfgbfdhnhdnd!\r\n";
////	std::s tring tmp3 = "PUT http://localhost:8080/post_body/1.php/user/bin/php?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
//	parseRequest(tmp3, req);
//
//
//	if(parsBeforeCGI(req, _serversList.front()) == 0) {
//		try {
//			CGI qqq(req, _serversList.front());
//			qqq.init(req, _serversList.front());
//			qqq.creatENV();
//			qqq.exec();
//		}
//		catch (std::exception &exception) {
//			std::cerr << "error : " << exception.what() << std::endl;
//			return 1;
//		}
//	}
//	return 0;
//}



int		findMaxSD(std::vector<Server> &servers){
	std::vector<Server>::iterator it = servers.begin();
	int maxSd = 0;
	for (; it != servers.end(); it++){
		if (it->get_maxSd() > maxSd)
			maxSd = it->get_maxSd();
	}
	return maxSd;
}

int main(){

	try{
		ConfigParser parser;
		if (!parser.parseConfig("webserv.conf")){
			std::cerr << "Some parser error!!!" << std::endl;
			return -1;
		}
		timeval time;
		time.tv_sec = 10;
		time.tv_usec = 0;
		std::vector<Server>::iterator it;
		std::vector<Server> serversList(parser.getServers());

		it = serversList.begin();
		for (; it != serversList.end(); it++){
			it->create_master_socket(AF_INET, SOCK_STREAM, 0);
			it->set_address_socket(it->get_ip().c_str(), atoi(it->get_port().c_str()), AF_INET);
			it->bind_master_socket();
			listen_socket(it->get_master_socket(), SOMAXCONN);
			it->fullConfigEnvironment();
		}
		fd_set	readfds;
		fd_set	writefds;
		int		max_sd = 0;

		bool loop = true;
		while (loop){
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);

			for (it = serversList.begin(); it != serversList.end(); it++){
				FD_SET(it->get_master_socket(), &readfds);
				it->FD_reset(&readfds);
				it->FD_reset(&writefds);
			}
			max_sd = findMaxSD(serversList);

			int	ret = select(max_sd + 1, &readfds, &writefds, NULL, &time);
			if (ret == 0){
				std::cout << "Time out" << std::endl;
				continue;
			}
			else if (ret < 0)
				throw std::exception();

			it = serversList.begin();
			for (; it != serversList.end(); it++){
				if (FD_ISSET(it->get_master_socket(), &readfds)){
					int new_connection = it->accept_master_socket();
					fcntl(new_connection, F_SETFL, O_NONBLOCK);
					it->add_client(new_connection);
				}
				for (size_t i = 0; i < it->get_clientCount(); i++){
					int		sd = it->get_clientsd(i + 1);
					std::string buf(1024, '\0');
					Client &tmp = it->get_Client(i + 1);
					if (FD_ISSET(sd, &readfds)){
						if ((ret = read(sd, (void *)buf.c_str(), 1024)) == 0){
							close(sd);
							it->delete_client(i + 1);
						}
						else {
							std::string tmp3 = "POST http://localhost:8081/html/YoupiBanane/1.bla/CGI/cgi_tester?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nAuthorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l\r\n\r\nhello denis!\r\n";
							tmp.setFlag(parseRequest(tmp3, tmp.getRequest()));
							if (tmp.getFlag() == WAIT)
								continue;
							else if (tmp.getFlag() == ERROR){
								Response resp(400, *it);
							}
							else if (tmp.getFlag() == SEND){

							}
							std::cout << buf << std::endl;
						}
					}
					if (FD_ISSET(sd, &writefds)){
//						continue;
						if(parsBeforeCGI(tmp, *it) == 0) {
							try {
								CGI qqq(tmp.getRequest(), *it);
								qqq.init(tmp.getRequest(), *it);
								qqq.creatENV();
								qqq.exec();
							}
							catch (std::exception &exception) {
								std::cerr << "error : " << exception.what() << std::endl;
								return 1;
							}
						}
						exit(1);
						// if (write == yes){
						// 	send();
						// }
						// writefds.fds_bits;
//						send(sd, "Asd\r\n\r\n", 7, 0);
						// delete sd writefds;
					}
				}
			}
		}
	}
	catch(const std::exception& e){
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}