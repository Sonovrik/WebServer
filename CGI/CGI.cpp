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
//PUT localhost:8080/put_test/1.php/user/bin/php?q=r&a=d HTTP/1.1
///put_test/html/1.php : argv[1]


#include "CGI.hpp"

CGI::CGI(Request &req, Server &ser): env (NULL), envCount (17), RequestBody(""), ResponseBody("") {
	std::string envKey[] = {"AUTH_TYPE", "CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO",
	"PATH_TRANSLATED","QUERY_STRING", "REMOTE_ADDR", "REMOTE_IDENT", "REMOTE_USER", "REQUEST_METHOD", "REQUEST_URI",
	"SCRIPT_NAME", "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWARE"};
	for (int i = 0; i < envCount ; ++i) {
		this->envMap[envKey[i]];
	}
	std::map<std::string, std::string> tmp = req.getHeaders();
	for (std::map<std::string, std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		std::string a = it->first;
		if(a.find("-") != std::string::npos)
			a.replace(a.find("-"), 1, "_");
		std::transform(a.begin(), a.end(), a.begin(), toupper);
		envMap["HTTP_" + a] = it->second;
//		std::cout << envMap.find("HTTP_" + a)->first << envMap.find("HTTP_" + a)->second << std::endl;
	}

}

CGI::~CGI() {
	if(env){
		for (int i = 0; i < envCount; ++i) {
			delete env[i];
		}
		delete env;
	}
	if(argv){
		delete argv[0]; delete argv[1]; delete argv;
	}
}

CGI::CGI(const CGI &copy) {
}

CGI &CGI::operator=(const CGI &copy) {
	return *this;
}


void CGI::init(Request &req, Server &ser) {
//	http://localhost/1.cgi/path/to/interpretier?a=b (execute by cgi_tester)
//	this->envMap["CONTENT_LENGTH"] =  	const_cast<std::map<std::string,std::string>&>(req.getHeaders())["CONTENT-LENGTH"];
//	сортировка по методам и в зависимости от этого установка значение переменных

//	std::string method = req.getMethod();
//	if(method == "GET" || method == "HEAD")
//		; //Взять данные из переменной окружения QUERY_STRING
//	else if(method == "POST")
//		;Проанализировать переменную QUERY_STRING
//         Получить длинну данных из CONTENT_LENGTH
//         Если (CONTENT_LENGTH>0) то
//             Считать CONTENT_LENGTH байт из sdtin как данные.
//	else
//		; //error


	this->envMap["REQUEST_URI"] = req.getPath();             //"localhost/1.cgi";
	this->envMap["QUERY_STRING"] = "";                       //req.getQueryString() - это мапа?;     // "a=b"; //
	this->envMap["SCRIPT_NAME"] = "CGI.cpp";                 //?? req.getPath() ??
	this->envMap["PATH_INFO"] = "CGI/cgi_tester"; //req.getPathInfo(); //по дефолту "cgi_tester" or path/to/interpretier

	int r = req.getHeaders().find("AUTHORIZATION")->second.find(" ");
	this->envMap["AUTH_TYPE"] = req.getHeaders().find("AUTHORIZATION")->second.substr(0, r);                     //?? default Basic,
//	std::string tmpFoIdent = req.getHeaders().find("AUTHORIZATION")->second.substr(r + 1);
//	{
//		дешифратор! из "wergetrhjtuktulruyk" => "aladdin:opensesame";
//		this->envMap["REMOTE_IDENT"] = "passwd";                 //  "opensesame"
//		this->envMap["REMOTE_USER"] = "user";                    //  "aladdin"
//	}

	this->envMap["SERVER_SOFTWARE"] = "TOXIGEN";             // ?? версия нашего сервера
	this->envMap["SERVER_PROTOCOL"] = "HTTP/1.1";            //
	this->envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->envMap["REQUEST_METHOD"] = req.getMethod();        //"GET";
	if (req.getHeaders().find("CONTENT-LENGTH") != req.getHeaders().end())
		this->envMap["CONTENT_LENGTH"] = req.getHeaders().find("CONTENT-LENGTH")->second; // ?? если нет то??
	if (req.getHeaders().find("CONTENT-TYPE") != req.getHeaders().end())
		this->envMap["CONTENT_TYPE"] = req.getHeaders().find("CONTENT-TYPE")->second;   //"plain/text";  ""

	char dir[1024];
	getcwd(dir, 1024);
	this->envMap["PATH_TRANSLATED"] = std::string(dir) + this->envMap["PATH_INFO"];// ?? getcwd() + "cgi_tester" or /full/path/to/interpretier

	this->envMap["REMOTE_ADDR"] = "127.0.0.1";               // IP-adr клиента из запроса Den
	this->envMap["SERVER_NAME"] = "1.cgi";                   //  DEn наш порт в хедерах: HOST = <server-name> ":" <server-port>
	this->envMap["SERVER_PORT"] = "8081";                    //?? DEn наш порт в хедерах: HOST = <server-name> ":" <server-port>
	//
	this->RequestBody = req.getBody();
	this->argv = new char *[3];
	this->argv[0] = strdup("CGI/cgi_tester"); //strdup(envMap.find("PATH_INFO")->second.c_str()); // "cgi_tester" ""
//	std::cout << std::endl << this->argv[0] << std::endl;

	this->argv[1] = strdup("CGI//1.php");
	this->argv[2] = NULL;

}


void CGI::creatENV() {
	std::string tmp;
	env = new char *[envCount];
	std::map <std::string, std::string>:: iterator it = envMap.begin();
	for (int i = 0; it != envMap.end(); it++, i++) {
		tmp = it->first + "=\"" + it->second + "\"";
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
	int fdF = open("./myFile", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXO | S_IRWXG);
	int status;
	if (pipe(fd) != 0) {
		std::cerr << "cannot pipe" << std::endl;
		throw std::runtime_error("cannot pipe");
	}
	pid = fork();
	if(pid < 0) {
		throw std::runtime_error("cannot pid");
	}
	else if(pid == 0) // ребенок
	{
		close(fd[1]);
		if (dup2(fd[0], STDIN) < 0)
			std::cerr << "Cannot dup, 1" << std::endl;
		if (dup2(fdF, STDOUT) < 0)
			std::cerr << "Cannot dup, 2" << std::endl;
		std::cerr << "Doing..." << std::endl;
		ex = execve("/usr/bin/php", argv , NULL);
	}
	else // родитель
	{
//		write(fd[1], "dfdfdg", 6);
//		close(fd[1]);
		std::cerr << "Waiting..." << std::endl;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			status = WEXITSTATUS(status);
		}
		std::cerr << "Awaited..." << std::endl;
		std::cout << "status: " << status << std::endl;
		if(status != 0)
			;//gener.error response
//		close(fd[0]);
		close(fdF);
		close(fd[0]);
	}
}





