//
// Created by Kory Moaning on 3/2/21.
//

#include "CGI.hpp"

void parsBeforeCGI(Request &req, Server &ser){

}


int main(){

	ConfigParser parser;
	parser.parseConfig("../ConfigParser/webserv.conf");
	std::vector<Server> _serversList(parser.getServers());
	Request req;
	std::string tmp3 = "GET 1.php?a=b HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
//	std::string tmp3 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
	req = parseRequest(tmp3);

	// Для передачи запроса в CGI необходимо чекнуть расширение в
	// PATH (для тестера ".bla" либо это наше дефолтное расшерение из конфига ) &&  один из методов GET/POST/HEAD
	//
	// Создать и определиться с рабочей дефолтной директорией для cgi. "Все скрипты, как правило, помещают в директорию cgi-bin сервера,
	// но это не обязательно,в принципе скрипт может располагаться где угодно только при этом большинство Web-серверов требуют специальной настройки"
	//
	//если прилетает несколько запросов - не лезть в одну и ту же область памяти (в одни файлы)

	parsBeforeCGI(req, _serversList.front());

	CGI qqq(req, _serversList.front());
	qqq.init(req, _serversList.front());
	qqq.creatENV();
	qqq.exec();

	//на выходе после выполнения скрипта формируется боди и возможно часть хедеров для RESPONSE
	return 0;
}
