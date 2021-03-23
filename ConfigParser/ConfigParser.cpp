#include "ConfigParser.hpp"

const std::array<std::string, 2> ConfigParser::_blocks = {"server", "location"};

const std::array<std::string, 5> ConfigParser::_directives = {"server_name", "listen",
												"root", "error_page", "max_body_size"};

const std::array<std::string, 8> ConfigParser::_localDirectives = {"index", "root", "method",
												"max_body_size", "autoindex", "cgi_extensions",
												"cgi_path", "upload_storage"};

std::vector<Server>		ConfigParser::getServers(void) const{
	return _serversList;
}

int						ConfigParser::get_NumberOfServers(void) const{
	return _numberOfServers;
}

ConfigParser::ConfigParser():
	_tokens(NULL),
	_numberOfServers(0),
	_countLines(0){}


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
	std::string line("");
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
	size_t		b = 0;
	size_t		len = 0;
	std::string	tmp("");
	
	std::vector<std::string>::iterator it = _lines.begin();
	for (; it < _lines.end(); it++){
		tmp = *it;
		len = tmp.size();
		for (size_t i = 0; i < len; i++){
			if (!isascii(tmp[i]) || tmp[i] == '\'' || tmp[i] == '\"'
				|| tmp[i] == '\\' || tmp[i] == ';' || tmp[i] == '#')
				return false;
			else if (tmp[i] == '{'){
				if (!isInArray(_blocks.begin(), _blocks.end(), tmp) || i != len - 1)
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

std::vector<std::string>	ConfigParser::getTokens(std::string str){
	std::string					tmp("");
	std::vector<std::string>	tokens;

	for (size_t i = 0; i < str.size(); i++){
		if (str[i] == '{' || str[i] == '}' || str[i] == '\t' || str[i] == ' '){
			if (tmp.empty() == 0){
				tokens.push_back(tmp);
				tmp.clear();
			}
		}
		else
			tmp.push_back(str[i]);
		switch (str[i]){
			case '{':
				tokens.push_back("{");
				break;
			case '}':
				tokens.push_back("}");
				break;
			default:
				break;
		}
	}

	if (tmp.empty() == 0)
		tokens.push_back(tmp);
	return tokens;
}

bool	ConfigParser::checkBlocks(void){
	size_t		b = 0;
	size_t		i = 0;
	std::vector<std::string>::iterator	it;

	while (i < _countLines){
		it = _tokens[i].begin();
		if (_tokens[i].size() != 2 || *it != "server" || *(it + 1) != "{")
			return false;
		_numberOfServers++;
		b = 1;
		i++;
		while(b != 0){
			it = _tokens[i].begin();
			if ((find(it, _tokens[i].end(), "{")) != _tokens[i].end()){
				if (_tokens[i].size() != 3 || *it != "location" || *(it + 2) != "{" || b != 1)
					return false;
				b++;
			}
			if ((it = find(_tokens[i].begin(), _tokens[i].end(), "}")) != _tokens[i].end())
				b--;
			i++;
		}
	}

	if (_numberOfServers == 0)
		return false;
	return true;
}

bool	ConfigParser::fullTokens(void){
	_tokens = new std::vector<std::string>[_lines.size()];
	std::vector<std::string>::iterator it = _lines.begin();
	size_t	i = 0;
	while (it != _lines.end()){
		_tokens[i] = getTokens(*it);
		i++;
		it++;
	}
	_countLines = i;
	return true;
}

bool	ConfigParser::getLocation(size_t *index, location_t &location){
	size_t	i = *index;
	location._name = *(_tokens[i].begin() + 1);
	while (_tokens[++i].back() != "}"){
		std::pair<std::string, std::string>	p;
		p.first = _tokens[i].front();
		for (std::vector<std::string>::iterator it = _tokens[i].begin() + 1; it != _tokens[i].end(); it++){
			p.second += *it;
			if (it + 1 != _tokens[i].end())
				p.second.push_back(' ');
		}
		if (location._directives.find(p.first) != location._directives.end()
			|| !isInArray(_localDirectives.begin(), _localDirectives.end(), p.first))
			return false;
		location._directives.insert(p);
	}
	*index = i;
	return true;
}

bool	ConfigParser::pushDirective(Server	&serv, size_t index){
	int a = 0;
	if (_tokens[index].front() == "server_name" && serv.get_serverName().empty())
		a = 1;
	else if (_tokens[index].front() == "root" && serv.get_root().empty())
		a = 2;
	else if (_tokens[index].front() == "max_body_size" && serv.get_maxBodySize().empty())
		a= 3;
	else if (_tokens[index].front() == "listen" && serv.get_ip().empty())
		a = 4;
	else if (_tokens[index].front() == "error_page" && serv.get_errorPath(atoi((*(_tokens[index].begin() + 1)).c_str())).empty())
		a = 5;
	if ((a == 5 && _tokens[index].size() != 3) ||
		((a == 0 || _tokens[index].size() != 2) && a != 5))
		return false;

	int		val = 0;
	size_t	pos = 0;
	switch (a){
		case 1:
			serv.set_serverName(_tokens[index].back());
			break;
		case 2:
			serv.set_root(_tokens[index].back());
			break;
		case 3:
			val = atoi(_tokens[index].back().c_str());
			if (val < 8000 || val > 100000)
				return false;
			serv.set_maxBodySize(_tokens[index].back());
			break;
		case 4:
			if ((pos = _tokens[index].back().find(':')) == std::string::npos)
				return false;
			serv.set_ip(_tokens[index].back().substr(0, pos));
			serv.set_port(_tokens[index].back().substr(pos + 1, _tokens[index].back().size()));
			break;
		case 5:
			serv.add_errorPage(std::make_pair(atoi((*(_tokens[index].begin() + 1)).c_str()), _tokens[index].back()));
			std::string page = _tokens[index].back();
			if (page.rfind('.') == std::string::npos || 
				(std::string)&(page.c_str()[page.rfind('.')]) != ".html")
				return false;
			std::ifstream in(page);
			if (in.is_open() == false)
				return false;
			in.close();
			break;
	}
	return true;
}

bool	ConfigParser::checkURIS(std::vector<location_t>	&locations) const{
	std::vector<location_t>::iterator it = locations.begin();
	std::vector<location_t>::iterator it2;

	if (locations.size() == 0)
		return false;
	for (; it != locations.end(); it++){
		it2 = it + 1;
		for (; it2 != locations.end(); it2++){
			if (it2->_name == it->_name)
				return false;
		}
	}
	return true;
}


bool	ConfigParser::fullServers(void){
	size_t	j = 0;
	for (size_t i = 0; i < _numberOfServers; i++){
		Server					serv;
		std::vector<location_t>	locations;

		while (++j < _countLines && _tokens[j].back() != "}"){
			if (_tokens[j].back() == "{"){
				location_t loc;
				if (!getLocation(&j, loc))
					return false;
				locations.push_back(loc);
			}
			else if (!pushDirective(serv, j))
					return false;
		}
		j++;
		if (!checkURIS(locations))
			return false;
		serv.set_locations(locations);
		serv.fullBasicDirectives();
		_serversList.push_back(serv);
	}

	return true;
}

bool	ConfigParser::checkMainDirectives(void){
	std::vector<Server>::iterator	it = _serversList.begin();
	for (; it != _serversList.end(); it++){
		if (it->get_ip().empty() || it->get_root().empty())
			return false;
	}
	return true;
}

bool	ConfigParser::parseConfig(const std::string &fileName){
	if (!readConfig(fileName) || !checkBrackets()
		|| !fullTokens() || !checkBlocks())
		return false;
	std::cout << "OK 1" << std::endl;

	if (!fullServers() || !checkMainDirectives())
		return false;
	std::cout << "OK 2" << std::endl;

	_serversList.front().sortLocations();
	return true;
}
