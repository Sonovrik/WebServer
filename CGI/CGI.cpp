//
// Created by Kory Moaning on 2/26/21.
//


#include "CGI.hpp"

CGI::CGI(Request &req, Server &ser): env (NULL), envCount (16), RequestBody(""), ResponseBody("") {
	std::string envKey[] = {"AUTH_TYPE", "CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO",
	"PATH_TRANSLATED","QUERY_STRING", "REMOTE_ADDR", "REMOTE_IDENT", "REMOTE_USER", "REQUEST_METHOD",
	"REQUEST_URI", "SCRIPT_NAME", "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWARE"};
	for (int i = 0; i < envCount ; ++i) {
		this->envMap[envKey[i]];
	}
	std::map<std::string, std::string> tmp = req.getHeaders();
	for (std::map<std::string, std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		std::string a = it->first;
		if(a.find('-') != std::string::npos)
			a.replace(a.find('-'), 1, "_");
		std::transform(a.begin(), a.end(), a.begin(), toupper);
		envMap["HTTP_" + a] = it->second;
		envCount++;
	}
	dir = getcwd(NULL, 0);
	if (dir == NULL)
		throw std::runtime_error("500"); //("error getcwd. 500 Internal Server Error");
}

CGI::~CGI() {
	if(env) {
		for (int i = 0; i < envCount; ++i) {
			delete env[i];
		}
		delete env;
	}
	if(argv) {
		delete argv[0]; delete argv[1]; delete argv;
	}
	if(dir)
		delete dir;
}

CGI::CGI(const CGI &copy) {
}

CGI					&CGI::operator=(const CGI &copy) {
	return *this;
}

const std::string	&CGI::getResponseBody() const {
	return ResponseBody;
}

void				CGI::setResponseBody(const std::string &responseBody) {
	ResponseBody = responseBody;
}

void				CGI::setAuthorization(Request &req) {
	if(req.getHeaders().find("AUTHORIZATION") != req.getHeaders().end() &&
					!req.getHeaders().find("AUTHORIZATION")->second.empty()) {
		std::string tmp = req.getHeaders().find("AUTHORIZATION")->second;
		size_t pos = tmp.find(' ');
		if(pos != std::string::npos) {
			this->envMap["AUTH_TYPE"] = tmp.substr(0, pos);  //?? default Basic,
			std::string tmpFoIdent = tmp.substr(pos + 1);
			tmpFoIdent = b64decode(tmpFoIdent.c_str(), tmpFoIdent.size());
			if((pos = tmpFoIdent.find(':')) != std::string::npos) {
				this->envMap["REMOTE_USER"] = tmpFoIdent.substr(0, pos);
				this->envMap["REMOTE_IDENT"] = tmpFoIdent.substr(pos + 1);
			}
			else
				; //error
		}
		else
			; //error
	}
}

void				CGI::init(Request &req, Server &ser) {
	std::string pathCgi = req.getPath();
	pathCgi = pathCgi.erase(0, 1);
	this->envMap["REQUEST_URI"] = pathCgi;
	this->envMap["QUERY_STRING"] = req.getQueryString();
	this->envMap["SCRIPT_NAME"] = pathCgi;
	this->envMap["PATH_INFO"] = pathCgi;
	this->envMap["SERVER_SOFTWARE"] = ser.getEnvValue("SERVER_SOFTWARE");
	this->envMap["SERVER_PROTOCOL"] = ser.getEnvValue("SERVER_PROTOCOL");
	this->envMap["GATEWAY_INTERFACE"] = ser.getEnvValue("GATEWAY_INTERFACE");
	this->envMap["REQUEST_METHOD"] = req.getMethod();
	this->envMap["REMOTE_ADDR"] = ser.getEnvValue("REMOTE_ADDR");
	this->envMap["SERVER_NAME"] = ser.getEnvValue("SERVER_NAME");
	this->envMap["SERVER_PORT"] = ser.getEnvValue("SERVER_PORT");
	setAuthorization(req);
	if (req.getHeaders().find("CONTENT-LENGTH") != req.getHeaders().end())
		this->envMap["CONTENT_LENGTH"] = req.getHeaders().find("CONTENT-LENGTH")->second;
	else
		this->envMap["CONTENT_LENGTH"] = "";
	if (req.getHeaders().find("CONTENT-TYPE") != req.getHeaders().end())
		this->envMap["CONTENT_TYPE"] = req.getHeaders().find("CONTENT-TYPE")->second;
	else
		this->envMap["CONTENT_TYPE"] = "";
	this->envMap["PATH_TRANSLATED"] = this->envMap["PATH_INFO"][0] == '/' ?
		std::string(dir) + this->envMap["PATH_INFO"] : std::string(dir) + "/" + this->envMap["PATH_INFO"];
//	this->RequestBody = req.getBody();
	PathInfo = "/" + req.getPathInfo();
	req.setPathInfo(dir +  PathInfo);
	this->argv = new char *[3];
	this->argv[0] = strdup(( req.getPathInfo()).c_str());
	this->argv[1] = strdup((dir + envMap.find("REQUEST_URI")->second).c_str());  // проверка strdup
	this->argv[2] = NULL;
//	std::cout << "ARGV 0 " << this->argv[0] << std::endl;
//	std::cout << "ARGV 1 " << this->argv[1] << std::endl;
}

void				CGI::creatENV() {
	std::string tmp;
	envCount = envMap.size();
	env = new char *[envCount + 1]; // проверка маллокав
	std::map <std::string, std::string>:: iterator it = envMap.begin();
	for (int i = 0; it != envMap.end(); it++, i++) {
		tmp = it->first + "=" + it->second;
		env[i] = strdup(tmp.c_str()); // проверка маллокав
		if(!env[i])
			throw std::runtime_error("500");
	}
	env[envMap.size()] = NULL;
//	int i = 0;
//	while(i < envMap.size())
//	{
//		std::cout << "ENV "<< i << " " << env[i] << std::endl;
//		i++;
//	}
}

void				CGI::exec(Request &req) {
	pid_t pid;
	int ex;
	int fdF = open("./cgiFile", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXO | S_IRWXG);
	int status;
	if (pipe(fd) != 0)
		throw std::runtime_error("500");
	pid = fork();
	if(pid < 0) {
		throw std::runtime_error("500");
	}
	else if(pid == 0) { // ребенок
		close(fd[1]);
		if (dup2(fd[0], STDIN) < 0)
			throw std::runtime_error("500");
		if (dup2(fdF, STDOUT) < 0)
			throw std::runtime_error("500");
//		if(argv[0] != "/Users/kmoaning/Desktop/ToGit/cgi_tester")    //    чекнуть ENV для php-cgi
//			env = NULL;
		ex = execve(argv[0], argv , env);
		exit(ex);
	}
	else { // родитель
		close(fd[0]);
		write(fd[1], req.getBody().c_str(), req.getBody().length());
		close(fd[1]);
		std::cout << "Waiting..." << std::endl;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			status = WEXITSTATUS(status);
		}
		std::cout << "status: " << status << std::endl;
//		if(status != 0)
//			throw std::runtime_error("500");
		close(fdF); // положить в клиента не закрывая
		close(fd[0]);
	}
}


//	req.setPathInfo("/Users/kmoaning/Desktop/ToGit/cgi_tester"); //  /Users/kmoaning/.brew/bin/php-cgi