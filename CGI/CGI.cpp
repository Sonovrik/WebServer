//
// Created by Kory Moaning on 2/26/21.
//

//	AUTH_TYPE  - Если сервер поддерживает аутентификацию клиента и сценарий является защищенным сценарием,
//	эта переменная среды содержит метод, который используется для аутентификации клиента.
//	Пример: Cert_Or_Basic
//	Используется в случае если скрипт защищен от несанкционированного использования.
//	CONTENT_LENGTH - определяет размер данных в байтах, которые передаются скрипту.
//	Данная переменная чрезвычайно важна при обмене данными по методу POST,
//	т. к. нет другого способа определить размер данных, которые надо прочитать со стандартного ввода.
//	CONTENT_TYPE  определяет MIME-тип данных, передаваемых скрипту.
//	Используя эту переменную можно одним скриптом обрабатывать различные форматы данных.
//	GATEWAY_INTERFACE - определяет версию интерфейса.
//	PATH_INFO - передает программе путь, часть спецификации URL, в том виде, в котором она указана в клиенте.
//	Реально это означает, что передается путь (адрес скрипта) в виде, указанном в HTML-документе.
//	PATH_TRANSLATED - то же самое, что и PATH_INFO, но только после подстановки сервером определенных в его конфигурации вставок.
//	QUERY_STRING - переменная определяет содержание запроса к скрипту. Это строка запроса при методе !!!GET !!!
//	REMOTE_ADDR - IP-адрес запрашивающей машины.
//	REMOTE_IDENT - данная переменная порождается сервером, если он поддерживает идентификацию пользователя
//	по протоколу RFC-931. Рекомендовано использование этой переменной для первоначального использования скрипта.
//	REMOTE_USER - Если у вас есть защищенный сценарий и сервер поддерживает аутентификацию клиента,
//	эта переменная среды содержит имя пользователя, которое передается для аутентификации.
//	REQUEST_METHOD - определяет метод доступа к информационному ресурсу. Это важнейшая переменная в CGI.
//	Разные методы доступа используют различные механизмы передачи данных.
//	при GET запрос идет в переменную QUERY_STRING а при POST подается на STDIN скрипта и т.д
//	REQUEST_URI
//	SCRIPT_NAME - определяет адрес скрипта так, как он указан клиентом.
//	SERVER_NAME - определяет доменное имя сервера.
//	SERVER_PORT - - определяет порт TCP, по которому осуществляется взаимодействие. По умолчанию для работы
//	по HTTP используется 80 порт, но он может быть и переназначен при конфигурировании сервера.
//	SERVER_PROTOCOL - протокол сервера. Вообще говоря, CGI разрабатывалась не только для применения в www с
//	протоколом HTTP, но и для других протоколов также, но широкое применение получила только в www.
//	SERVER_SOFTWARE - определяет имя и версию сервера.
//	REMOTE_HOST - нет в сабже// - доменный адрес машины, с которой осуществляется запрос.
//
//
// PATH_TRANSLATED https://ru.wikipedia.org/wiki/Заглавная_страница, а PATH_INFO - то что придет от клиента на сервер, как перевести??
//



#include "CGI.hpp"

CGI::CGI(): env (NULL), envCount (17), body ("") {
	std::string envKey[] = {"AUTH_TYPE", "CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO",
	"PATH_TRANSLATED","QUERY_STRING", "REMOTE_ADDR", "REMOTE_IDENT", "REMOTE_USER", "REQUEST_METHOD", "REQUEST_URI",
	"SCRIPT_NAME", "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWARE"};
	for (int i = 0; i < envCount ; ++i) {
		this->envMap[envKey[i]];
	}
}

void CGI::init(Request &req) { //, Server &ser) {
//	http://localhost/1.cgi/path/to/interpretier?a=b (execute by cgi_tester)
//	сортировка по методам и в зависимости от этого установка значение переменных

//	std::string method = req.getMethod();
//	if(method == "GET" || method == "HEAD")
//		; Взять данные из переменной окружения QUERY_STRING
//	else if(method == "POST")
//		;Проанализировать переменную QUERY_STRING
//         Получить длинну данных из CONTENT_LENGTH
//         Если (CONTENT_LENGTH>0) то
//             Считать CONTENT_LENGTH байт из sdtin как данные.
//	else
//		; //error

	this->envMap["REQUEST_URI"] = "localhost/1.cgi"; //?? req.getPath() headers["LOCATION"];
	this->envMap["AUTH_TYPE"] = "Basic"; //?? req.getHeaders().find [] ?? default Basic, DEN ID/pass
	this->envMap["CONTENT_LENGTH"] = "100"; //?? headers["CONTENT-LENGTH"]; если нет то??
	this->envMap["CONTENT_TYPE"] = "plain/text"; //?? headers["CONTENT-TYPE"];
	this->envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->envMap["PATH_INFO"] = "cgi_tester"; // ?? // or path/to/interpretier
	this->envMap["PATH_TRANSLATED"] = ""; // ?? getcwd() + cgi_tester or /full/path/to/interpretier
	this->envMap["QUERY_STRING"] = "a=b"; //
	this->envMap["REMOTE_ADDR"] = "127.0.0.1"; // IP-adr клиента из запроса Den
	this->envMap["REMOTE_IDENT"] = "passwd"; // если есть аутентификация
	this->envMap["REMOTE_USER"] = "user"; // используется для идентификации пользователя
	this->envMap["REQUEST_METHOD"] = "GET"; //?? req.getMethod() ??
	this->envMap["SCRIPT_NAME"] = "CGI.cpp"; //?? req.getPath() ??
	this->envMap["SERVER_NAME"] = "1.cgi"; //  наш хост в хедерах у маши : HOST = <server-name> ":" <server-port>
	this->envMap["SERVER_PORT"] = "8080"; //?? DEn наш порт в хедерах у маши : HOST = <server-name> ":" <server-port>
	this->envMap["SERVER_PROTOCOL"] = "HTTP/1.1"; // ?? чья версия?? req.getVersion()
	this->envMap["SERVER_SOFTWARE"] = "TOXIGEN"; // ?? версия нашего сервера
//	this->body = "body"; // ?? req.getBody();
	this->argv = new char *[3];
	this->argv[0] = strdup("/usr/bin/php");
	this->argv[1] = strdup("../1.php");
	this->argv[2] = NULL;

}

char *myStrdup(std::string line){
	const char *res = new char;
	res = line.c_str();
	return ((char *)res);
}

void CGI::creatENV() {
	std::string tmp;
	env = new char *[envCount];
	std::map <std::string, std::string>:: iterator it = envMap.begin();
	for (int i = 0; it != envMap.end(); it++, i++) {
		tmp = it->first + "=\"" + it->second + "\"";
//		env[i] = myStrdup(tmp);
		env[i] = strdup(tmp.c_str());
	}

}

// содержимое скрипта поступает на fd[0](STDIN подменяется fd[0])
// результат на STDOUT(подменненный на fdF)
// в fd[1] - тело запроса

void CGI::exec() {
	//Программа должна быть или двоичным исполняемым файлом,
	//или скриптом, начинающимся со строки вида "#! интерпретатор [аргументы]"
	//В последнем случае интерпретатор -- это правильный путь к исполняемому
	//файлу, который не является скриптом; этот файл будет выполнен как интерпретатор [arg] filename.
	pid_t pid;
	int ex;
	pipe(fd);
	int status;
	pid = fork();
	if(pid < 0)
		; // error
	else if(pid == 0) // ребенок
	{
		close(fd[1]);
		int fdF = open("./aaa", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXO | S_IRWXG);
		dup2(fd[0], STDIN);
		dup2(fdF, STDOUT);
		ex = execve("/usr/bin/php", argv , NULL);
		close(fdF);
		close(fd[0]);
		exit(ex);
	}
	else // родитель
	{
//		write(fd[1], "dfdfdg", 6);
//		close(fd[1]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			status = WEXITSTATUS(status);
		}
		std::cout << "status: " << status << std::endl;
		//		close(fd[0]);
	}
}

CGI::~CGI() {
	if(env != NULL)
		delete env;
}

CGI::CGI(const CGI &copy) {

}

CGI &CGI::operator=(const CGI &copy) {
	return *this;
}




