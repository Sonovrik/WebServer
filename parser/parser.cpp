#include "Request.hpp"

const std::string Request::_methodsNames[] = {"GET", "HEAD",
			"POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};

bool	Request::parse_start_line(std::string str){
	std::string	token[3];
	size_t		pos;

	// parse parts of string in tokens
	for (int i = 0; i < 2; i++){
		if ((pos = str.find(' ')) == std::string::npos)
			return false;
		token[i] = str.substr(0, pos);
		str.erase(0, pos + 1);
	}
	token[2] = str.substr(0, str.length());

	// find methods
	for (int i = 0; i < _methodsNames->length(); i++){
		if (_methodsNames[i] == token[0])
			this->_method = token[0];
	}
	if (this->_method.empty())
		return false;
	this->_path = token[1];

	// please add envs in this string
	if (token[2] != "HTTP/1.1\r\n") 
		return false;
	this->_version = token[2].substr(0, token[2].length() - 2);
	return true;
}

void		trim_string(std::string &line){
	int i = 0;
	while (line[i] == ' ')
		i++;
	line.erase(0, i);
	
	i = line.length() - 1;
	while (line[i] == ' ')
		i--;
	line.erase(i + 1, line.length());
}

bool		Request::push_map_node(std::string line){ // rename please
	size_t		pos = 0;
	
	trim_string(line);
	if ((pos = line.find(':')) == std::string::npos)
		return false;
	std::pair<std::string, std::string> node;

	node.first = line.substr(0, pos);
	if (node.first.find(' ') != std::string::npos)
		return false;

	line.erase(0, pos + 1);
	trim_string(line);

	node.second = line;
	// if (node.first.find(' ') != std::string::npos || node.second.find(' ') != std::string::npos)
	// 	return false; // i don't know about whitespaces without ,; and e.t.c. also ASCII characters
	std::cout << node.first << node.second << std::endl;
	this->_headers.insert(node);
	return true;
}

bool		Request::parse_headers(std::string req){
	std::string	tmp;
	size_t		pos = 0;
	std::string	delimentr = "\r\n";

	while((pos = req.find(delimentr)) != std::string::npos && pos != req.length() && pos != 0){
		tmp = req.substr(0, pos);
		push_map_node(tmp);
		req.erase(0, pos + delimentr.size());
	}
	// POST request. Body read
	// if (pos == 0) ???

	return true;
	// if (pos = req.find("\r\n\r\n"));
}

Request		parse_request(std::string req){
	size_t		pos = 0;
	Request		ret;

	pos = req.find("\r\n");
	std::string	tmp(req.substr(0, pos + 2));
	if (ret.parse_start_line(tmp) == false)
		return ret; // error try catch ???
	req.erase(0, pos + 2);

	if (ret.parse_headers(req))
		return ret; // error try catch ???
	if (ret.get_method() == "POST"){
		// parse a body or wait a body
	}
	
	return ret;
}

int main(){
	Request req;
	std::string tmp = "POST /cgi-bin/process.cgi HTTP/1.1\n\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n\nHost: www.example.com\n\nContent-Type: application/x-www-form-urlencoded\n\n";
	std::string tmp2 = "POST / HTTP/1.1\r\n";
	std::string tmp3 = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:5991\r\nUser-Agent: curl/7.47.0\r\n    Accept: */*    \r\n\r\n";
	req = parse_request(tmp3);
	// if (req.parse_start_line(tmp2))
	// 	std::cout << "good" << std::endl;
	return 0;
}