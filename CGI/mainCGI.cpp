//
// Created by Kory Moaning on 3/2/21.
//


#include "CGI.hpp"

int main(){
	Request req;
//	std::string tmp = "POST /cgi-bin/process.cgi HTTP/1.1\n\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n\nHost: www.example.com\n\nContent-Type: application/x-www-form-urlencoded\n\n";
//	std::string tmp2 = "POST / HTTP/1.1\r\n";
	std::string tmp3 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\nAccept: */*    \r\n\r\n";
	req = parse_request(tmp3);
	// if (req.parse_start_line(tmp2))
	// 	std::cout << "good" << std::endl;

	// Для передачи запроса в CGI необходимо чекнуть расширение в
	// PATH (для тестера ".bla" либо это наше дефолтное расшерение из конфига ) &&  один из методов GET/POST/HEAD
	//
	// Создать и определиться с рабочей дефолтной директорией для cgi. "Все скрипты, как правило, помещают в директорию cgi-bin сервера,
	// но это не обязательно,в принципе скрипт может располагаться где угодно только при этом большинство Web-серверов требуют специальной настройки"
	//
	//если прилетает несколько запросов - не лезть в одну и ту же область памяти (в одни файлы)

	CGI qqq;
	qqq.init();
	qqq.creatENV();
	qqq.exec();

	//на выходе после выполнения скрипта формируется боди и возможно часть хедеров для RESPONSE
	return 0;
}
