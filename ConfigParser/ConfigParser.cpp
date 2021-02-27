#include "ConfigParser.hpp"

const std::array<std::string, 2> ConfigParser::_blocks = {"server", "location"};

void		printVector(std::vector<std::string> &vector){
	std::vector<std::string>::iterator it = vector.begin();
	for (; it != vector.end(); it++){
		std::cout << *it << std::endl;
	}
}

ConfigParser::ConfigParser():
	_tokens(NULL),
	_serverName(""),
	_ipToListen(""),
	_root(""),
	_maxBodySize(""),
	_errorPage(""){}


ConfigParser::~ConfigParser(){
	if (_tokens != NULL)
		delete []_tokens;
}

void		ConfigParser::trimString(std::string &line){
	size_t i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	line.erase(0, i);

	size_t len = line.length();
	i = len - 1;
	while (line[i] == ' ' || line[i] == '\t')
		i--;
	if (i < len)
		line.erase(i + 1, len);
}

bool	ConfigParser::readConfig(const std::string &fileName){
	std::string line;
	std::ifstream in(fileName);

	if (in.is_open() == false)
		return false;
	while (getline(in, line)){
		trimString(line);
		if (line.size() != 0)
			_lines.push_back(line);
	}
	in.close();
	return true;
}

bool			ConfigParser::checkBrackets(void){
	size_t	b = 0;
	size_t	len = 0;
	size_t	pos = 0;
	std::string		tmp;
	
	std::vector<std::string>::iterator it = _lines.begin();
	for (; it < _lines.end(); it++){
		tmp = *it;
		len = tmp.size();
		for (size_t i = 0; i < len; i++){
			if (!isascii(tmp[i]) || tmp[i] == '\'' || tmp[i] == '\"' || tmp[i] == '\\')
				return false;
			else if (tmp[i] == '{'){
				if (isInArray(_blocks.begin(), _blocks.end(), tmp) == false
					|| i != len - 1)
					return false;
				b++;
			}
			else if (tmp[i] == '}'){
				if (len != 1)
					return false;
				b--;
			}
		}
	}
	if (b != 0)
		return false;

	return true;
}

std::vector<std::string>	ConfigParser::getTokens(std::vector<std::string>::iterator &it){
	std::string		tmp("");
	std::vector<std::string>	tokens;
	std::string		line = *it;
	for (int i = 0; i < line.size(); i++){
		if (line[i] == '{' || line[i] == '}' || line[i] == ';' 
			|| line[i] == '\t' || line[i] == ' '){
			if (tmp.empty() == 0){
				tokens.push_back(tmp);
				tmp.clear();
			}
		}
		else
			tmp += line[i];
		switch (line[i]){
			case '{':
				tokens.push_back("{");
				break;
			case '}':
				tokens.push_back("}");
				break;
			case ';':
				tokens.push_back(";");
				break;
			default:
				break;
		}
	}
	if (tmp.empty() == 0)
		tokens.push_back(tmp);
	
	return tokens;
}

bool	ConfigParser::getNumberServers(){
	size_t		b = 0;
	std::vector<std::string>::iterator it;
	for (size_t i = 0; i < _countLines; i++){
		it = _tokens[i].begin();
		if (_tokens[i].size() != 2 || *it != "server" || *(it + 1) != "{")
			return false;
		b = 1;
		i++;
		while(b != 0){
			it = _tokens[i].begin();
			if ((find(it, _tokens[i].end(), "{")) != _tokens[i].end()){
				if (_tokens[i].size() != 3 || *it != "location" || *(it + 2) != "{")
					return false;
				// need parse to locations[]
				b++;
			}
			if ((it = find(_tokens[i].begin(), _tokens[i].end(), "}")) != _tokens[i].end())
				b--;
			i++;
		}
		i--;
	}
	return true;
}

bool	ConfigParser::fullTokens(void){
	_tokens = new std::vector<std::string>[_lines.size()];
	std::vector<std::string>::iterator it = _lines.begin();
	size_t	i = 0;
	while (it != _lines.end()){
		_tokens[i] = getTokens(it);
		// printVector(_tokens[i]);
		i++;
		it++;
	}
	_countLines = i;
	if (getNumberServers() == false){
		return false;
	}
	return true;
}

bool	ConfigParser::parseConfig(const std::string &fileName){
	size_t	pos = 0;
	std::string		tmp;
	
	if (!readConfig(fileName) || !checkBrackets())
		return false;
	// if ()
	if (fullTokens() == false)
		return false;
	std::cout << "OK " << std::endl;
	// std::vector<std::string>::iterator it = _lines.begin();
	// for (; it < _lines.end(); it++){
	// 	std::cout << *it << std::endl;
	// }
	
	return true;
}

int		main(){
	ConfigParser parser;
	parser.parseConfig("webserv.conf");
	return 0;
}