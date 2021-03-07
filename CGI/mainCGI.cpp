//
// Created by Kory Moaning on 3/2/21.
//

#include "CGI.hpp"
#include "../utils.hpp"

// Для передачи запроса в CGI необходимо чекнуть расширение в
// PATH (для тестера ".bla" либо это наше дефолтное расшерение из конфига ) &&  один из методов GET/POST/HEAD
//
// Создать и определиться с рабочей дефолтной директорией для cgi. "Все скрипты, как правило, помещают в директорию cgi-bin сервера,
// но это не обязательно,в принципе скрипт может располагаться где угодно только при этом большинство Web-серверов требуют специальной настройки"
//
//если прилетает несколько запросов - не лезть в одну и ту же область памяти (в одни файлы)
//на выходе после выполнения скрипта формируется боди и возможно часть хедеров для RESPONSE
//PUT http://localhost:8080/put_test/1.php/user/bin/php?q=r&a=d HTTP/1.1
///put_test/html/1.php : argv[1]

std::string checkLocation(std::string &uri, Server &ser){
	std::string res = "";
	std::vector<location_t> tmp = ser.get_locations();
	size_t count = tmp.size();
	size_t len;
	int i = 0;
	while (i < count) {
		len = tmp[i]._name.size();
		if(uri.compare(0, len, tmp[i]._name) == 0){
			res = tmp[i]._directives.find("root")->second + tmp[i]._name;
			uri.erase(0, tmp[i]._name.length());
			break;
		}
		i++;
	}
	if(i == count)
		res = ser.get_root() + "/";
	return res;
}

std::string checkPath(std::string &uri, std::string &location){
	std::string res = location + uri;
	size_t len = res.length();
	size_t count = 0;
	struct stat info;
	for (int i = 0; i < len; ++i){
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
			if ((pos = res.find('/')) == std::string::npos){
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
	if(tmp[last] == '/'){  // что если придет запрос  http://localhost:8080/put_test/
		tmp.erase(tmp.length() - 1, 1);
		path = tmp.c_str();
		if(stat(path, &info) == 0){
			std::cout << "path found " << tmp << std::endl;
			return (tmp);
		}
	}
	else
		std::cout << "path found " << tmp << std::endl;
	return (tmp);
}

void parsBeforeCGI(Request &req, Server &ser){
	std::string uri = req.getPath();
	std::string host;
	std::string hostName;
	std::string pathToScript;
	std::string port;
	std::string location;
	if(uri.compare(0, 7, "http://") == 0)
		uri.erase(0, 7); //delete http://
	int pos;
	if((pos = uri.rfind('?')) != std::string::npos)
		uri.erase(pos);
	pos = 0;
	if((pos = uri.find('/')) != std::string::npos){
		if(pos != 1){  // как понять что это не 1.php/user/bin/php?q=r&a=d
			host.assign(uri, 0, pos);
			int i;
			if((i = host.find(':')) != std::string::npos) {
				hostName.assign(host, 0, i);
				port = host.substr(i + 1);
			}
			else
				hostName = host;
			uri.erase(0, host.length());
		}
		else
			;// не указан хост, возможно при GET /1.php
	}
	else
		;// не нашел '/' вообще, может ли быть GET 1.php
	location = checkLocation(uri, ser);
	pathToScript = checkPath(uri, location);

	std::cout << "uri : " << uri << std::endl << "path : " << req.getPath() << std::endl;
	std::cout << "host : " << host << std::endl << "hostName : " << hostName << std::endl;
	std::cout << "port : " << port << std::endl  << "location : " << location << std::endl;
	std::cout << "pathToScript : " << pathToScript << std::endl;
}


int main(){

	ConfigParser parser;
	parser.parseConfig("webserv.conf");
	std::vector<Server> _serversList(parser.getServers());
	for (std::vector<Server>::iterator it = _serversList.begin(); it < _serversList.end(); it++){
		it->fullConfigEnvironment();
	}

	Request req;
	std::string tmp3 = "PUT http://localhost:8080/put_test/1.php/user/bin/php?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
//	std::string tmp3 = "PUT http://localhost:8080/post_body/1.php/user/bin/php?q=r&a=d HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
//	std::string tmp3 = "GET 1.php?a=b HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
//	std::string tmp3 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
	parseRequest(tmp3, req);


	parsBeforeCGI(req, _serversList.front());
	CGI qqq(req, _serversList.front());
	qqq.init(req, _serversList.front());
	qqq.creatENV();
	qqq.exec();
	return 0;
}
