//
// Created by Kory Moaning on 2/26/21.
//


#include "CGI.hpp"

CGI::CGI(Request &req, Server &ser): env (NULL), envCount (16) {
	std::string envKey[] = {"AUTH_TYPE", "CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO",
	"PATH_TRANSLATED","QUERY_STRING", "REMOTE_ADDR", "REMOTE_IDENT", "REMOTE_USER", "REQUEST_METHOD",
	"REQUEST_URI", "SCRIPT_NAME", "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWARE"};
	std::map<std::string, std::string> tmp = req.getHeaders();
	std::string a;

	for (int i = 0; i < envCount ; ++i) {
		this->envMap[envKey[i]];
	}
	for (std::map<std::string, std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		a = it->first;
		if(a.find('-') != std::string::npos)
			a.replace(a.find('-'), 1, "_");
		std::transform(a.begin(), a.end(), a.begin(), toupper);
		envMap["HTTP_" + a] = it->second;
		envCount++;
	}
	if (!(dir = getcwd(NULL, 0)))
		throw std::runtime_error("500");
	body = req.getBody();
}

CGI::~CGI() {
	if(env) {
		for (int i = 0; i < envCount; ++i)
			delete env[i];
		delete env;
	}
	if(argv[0])
		delete argv[0];
	if(argv[1])
		delete argv[1];
	delete argv;
	if(dir)
		delete dir;
}

void		CGI::mallocError(int count)  {
	for (int i = 0; i < count; ++i) {
		delete env[i];
	}
	delete env;
	throw std::runtime_error("500");
}

void		CGI::setAuthorization(Request &req) {
	size_t pos;
	std::string tmpFoIdent;

	if(req.getHeaders().find("AUTHORIZATION") != req.getHeaders().end() &&
	!req.getHeaders().find("AUTHORIZATION")->second.empty()) {
		std::string tmp = req.getHeaders().find("AUTHORIZATION")->second;
		pos = tmp.find(' ');
		this->envMap["AUTH_TYPE"] = tmp.substr(0, pos);
		tmpFoIdent = tmp.substr(pos + 1);
		tmpFoIdent = b64decode(tmpFoIdent.c_str(), tmpFoIdent.size());
		pos = tmpFoIdent.find(':');
		this->envMap["REMOTE_USER"] = tmpFoIdent.substr(0, pos);
		this->envMap["REMOTE_IDENT"] = tmpFoIdent.substr(pos + 1);
	}
}

void		CGI::init(Request &req, Server &ser) {
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
	PathInfo = req.getPathInfo();
	if(req.getPathInfo().compare(0, 1,"."))
		PathInfo = "/" + req.getPathInfo();
	req.getPathInfo() == "cgi_tester" ? req.setPathInfo(dir + PathInfo) : req.setPathInfo(PathInfo);
	this->argv = new char *[3];
	if(!(this->argv[0] = strdup(req.getPathInfo().c_str())))
		throw std::runtime_error("500");
	if(!(this->argv[1] = strdup((std::string(dir) + pathCgi).c_str())))
		throw std::runtime_error("500");
	this->argv[2] = NULL;
}

void		CGI::creatENV() {
	std::string	tmp;

	envCount = envMap.size();
	if(PathInfo != "/cgi_tester") {
		return ;
	}
	env = new char *[envCount + 1];
	std::map <std::string, std::string>:: iterator it = envMap.begin();
	for (int i = 0; it != envMap.end(); it++, i++) {
		tmp = it->first + "=" + it->second;
		if(!(env[i] = strdup(tmp.c_str())))
			mallocError(i);
	}
	env[envMap.size()] = NULL;
}

void		CGI::exec(Request &req, Server &ser) {
	pid_t	pid;
	ssize_t	ret;
	int		fdF;
	int		status;

	init(req, ser);
	creatENV();
	if ((fdF = open("./cgiFile", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXO | S_IRWXG)) == -1)
		throw std::runtime_error("500");
	if (pipe(fd) != 0)
		throw std::runtime_error("500");
	pid = fork();
	if(pid < 0) {
		throw std::runtime_error("500");
	} else if(pid == 0) {
		close(fd[1]);
		if (dup2(fd[0], STDIN) < 0)
			throw std::runtime_error("500");
		if (dup2(fdF, STDOUT) < 0)
			throw std::runtime_error("500");
		exit(execve(argv[0], argv , env));
	} else {
		close(fd[0]);
		while ((ret = write(fd[1], body.c_str(), body.size())) > 0)
			body.erase(0, ret);
		if(ret == -1)
			throw std::runtime_error("500");
		close(fd[1]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		if(status != 0)
			throw std::runtime_error("500");
		close(fdF);
		close(fd[0]);
	}
}
