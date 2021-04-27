#include "ConfigParser.hpp"

//////////////////////////   Constants   \\\\\\\\\\\\\\\\\\\\\\\\\\

static const std::array<std::string, 2> blocks = {"server", "location"};

static const std::array<std::string, 6> directives = {"server_name", "listen",
												"root", "error_page", "max_body_size", "max_buffer_size"};

static const std::array<std::string, 11> localDirectives = {"index", "root", "method",
												"max_body_size", "autoindex", "cgi_extensions",
												"cgi_path", "upload_storage", "return", "auth_basic",
												"auth_basic_user_file"};

//////////////////////////   Constants   \\\\\\\\\\\\\\\\\\\\\\\\\\



//////////////////////////   Coplin   \\\\\\\\\\\\\\\\\\\\\\\\\\

ConfigParser::ConfigParser():
	_tokens(NULL),
	_numberOfServers(0),
	_countLines(0){}


ConfigParser::~ConfigParser(){
	if (this->_tokens != NULL){
		delete [](this->_tokens);
	}
	this->_lines.clear();
	this->_serversList.clear();
}

//////////////////////////   Coplin   \\\\\\\\\\\\\\\\\\\\\\\\\\

//////////////////////////   Checkers   \\\\\\\\\\\\\\\\\\\\\\\\\\

bool			ConfigParser::checkBrackets(void) const{
	size_t		b = 0;
	size_t		len = 0;
	std::string	tmp("");
	std::vector<std::string>::const_iterator it = this->_lines.begin();
	
	for (; it < this->_lines.end(); it++){
		tmp = *it;
		len = tmp.size();
		for (size_t i = 0; i < len; i++){
			if (!isascii(tmp[i]) || tmp[i] == '\'' || tmp[i] == '\"'
				|| tmp[i] == '\\' || tmp[i] == ';' || tmp[i] == '#')
				return false;
			else if (tmp[i] == '{'){
				if (!isInArray(blocks.begin(), blocks.end(), tmp) || i != len - 1)
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

bool	ConfigParser::checkBlocks(void){
	size_t		b = 0;
	size_t		i = 0;
	std::vector<std::string>::iterator	it;

	while (i < this->_countLines){
		it = this->_tokens[i].begin();
		if (this->_tokens[i].size() != 2 || *it != "server" || *(it + 1) != "{")
			return false;
		this->_numberOfServers++;
		b = 1;
		i++;
		while(b != 0){
			it = this->_tokens[i].begin();
			if ((find(it, this->_tokens[i].end(), "{")) != this->_tokens[i].end()){
				if (this->_tokens[i].size() != 3 || *it != "location" || *(it + 2) != "{" || b != 1)
					return false;
				b++;
			}
			if ((it = find(this->_tokens[i].begin(), this->_tokens[i].end(), "}")) != this->_tokens[i].end())
				b--;
			i++;
		}
	}
	if (this->_numberOfServers == 0)
		return false;
	return true;
}


bool	ConfigParser::checkMainDirectives(void) const{
	std::vector<Server>::const_iterator	it = this->_serversList.begin();

	for (; it != this->_serversList.end(); it++){
		if (it->get_ip().empty() || it->get_root().empty())
			return false;
	}
	return true;
}

bool	ConfigParser::checkURIS(std::vector<location_t>	&locations) const{
	std::vector<location_t>::const_iterator it = locations.begin();
	std::vector<location_t>::const_iterator it2;

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

bool			ConfigParser::checkLocations(std::vector<location_t>	&locations) const{
	struct stat st;
	if (!checkURIS(locations))
		return false;
	std::map<std::string, std::string>::const_iterator it2;
	std::vector<location_t>::const_iterator it = locations.begin();
	for (; it != locations.end(); it++){
		if (it->_directives.find("auth_basic") == it->_directives.end()
			&& it->_directives.find("auth_basic_user_file") == it->_directives.end())
			return true;
		if (it->_directives.find("auth_basic") != it->_directives.end()
			&& it->_directives.find("auth_basic_user_file") != it->_directives.end()
			&& stat(it->_directives.find("auth_basic_user_file")->second.c_str(), &st) != -1)
			return true;
		break;
	}
	return false;
}


//////////////////////////   Checkers   \\\\\\\\\\\\\\\\\\\\\\\\\\



//////////////////////////   Init   \\\\\\\\\\\\\\\\\\\\\\\\\\

bool	ConfigParser::readConfig(const std::string &fileName){
	std::string line("");
	std::ifstream in(fileName);

	if (in.is_open() == false)
		return false;
	while (getline(in, line)){
		trimString(line);
		if (line.size() != 0)
			this->_lines.push_back(line);
	}
	in.close();
	return true;
}

bool	ConfigParser::parseConfig(const std::string &fileName){
	if (!readConfig(fileName) || !checkBrackets()
		|| !fullTokens() || !checkBlocks())
		throw ConfigErrorException();

	if (!fullServers() || !checkMainDirectives())
		throw ConfigErrorException();

	for (std::vector<Server>::iterator it = this->_serversList.begin(); it != this->_serversList.end(); it++)
		it->sortLocations();
	return true;
}

bool	ConfigParser::fullTokens(void){
	size_t	i = 0;
	this->_tokens = new std::vector<std::string>[this->_lines.size()];
	std::vector<std::string>::iterator it = this->_lines.begin();

	while (it != this->_lines.end()){
		this->_tokens[i] = getTokens(*it);
		if (this->_tokens[i].front() != "}" && this->_tokens[i].size() < 2)
			return false;
		i++;
		it++;
	}
	this->_countLines = i;
	return true;
}

bool	ConfigParser::fullLocation(location_t &location){
	return true;
}


bool	ConfigParser::getLocation(size_t &index, location_t &location){
	location._name = *(this->_tokens[index].begin() + 1);
	while (this->_tokens[++index].back() != "}"){
		std::pair<std::string, std::string>	p;
		p.first = this->_tokens[index].front();
		for (std::vector<std::string>::iterator it = this->_tokens[index].begin() + 1; it != this->_tokens[index].end(); it++){
			p.second += *it;
			if (it + 1 != this->_tokens[index].end())
				p.second.push_back(' ');
		}
		if (location._directives.find(p.first) != location._directives.end()
			|| !isInArray(localDirectives.begin(), localDirectives.end(), p.first))
			return false;
		location._directives.insert(p);
	}

	// fillLocation

	return true;
}

bool	ConfigParser::pushDirective(Server	&serv, size_t index){
	int a = 0;
	long long val = 0;
	size_t	pos = 0;
	struct stat st;

	if (this->_tokens[index].front() == "server_name" && serv.get_serverName().empty())
		a = 1;
	else if (this->_tokens[index].front() == "root" && serv.get_root().empty())
		a = 2;
	else if (this->_tokens[index].front() == "max_body_size" && serv.get_maxBodySize().empty())
		a= 3;
	else if (this->_tokens[index].front() == "listen" && serv.get_ip().empty())
		a = 4;
	else if (this->_tokens[index].front() == "error_page" && serv.get_errorPath(atoi((*(this->_tokens[index].begin() + 1)).c_str())).empty())
		a = 5;
	else if (this->_tokens[index].front() == "max_buffer_size" && serv.get_maxBufferSize().empty())
		a = 6;
	if ((a == 5 && this->_tokens[index].size() != 3) ||
		((a == 0 || this->_tokens[index].size() != 2) && a != 5))
		return false;
	switch (a){
		case 1:
			serv.set_serverName(this->_tokens[index].back());
			break;
		case 2:
			serv.set_root(this->_tokens[index].back());
			break;
		case 3:
			val = atoi(this->_tokens[index].back().c_str());
			if (val < 1000 || val > 1000000000)
				return false;
			serv.set_maxBodySize(this->_tokens[index].back());
			break;
		case 4:
			if ((pos = this->_tokens[index].back().find(':')) == std::string::npos)
				return false;
			serv.set_ip(this->_tokens[index].back().substr(0, pos));
			serv.set_port(this->_tokens[index].back().substr(pos + 1, this->_tokens[index].back().size()));
			break;
		case 6:
			val = atoi(this->_tokens[index].back().c_str());
			if (val < 1000 || val > 1000000000)
				return false;
			serv.set_maxBufferSize(this->_tokens[index].back());
			break;
		case 5:
			std::string	path = this->_tokens[index].back();
			if (stat(path.c_str(), &st) == -1)
				return false;
			if (path.rfind('.') == std::string::npos || 
				(std::string)&(path.c_str()[path.rfind('.')]) != ".html")
				return false;
			std::string page = getFileText(path);
			serv.add_errorPage(std::make_pair(atoi((*(this->_tokens[index].begin() + 1)).c_str()), page));
			break;
	}
	return true;
}

bool	ConfigParser::fullServers(void){
	size_t	j = 0;

	for (size_t i = 0; i < this->_numberOfServers; i++){
		Server					serv;
		std::vector<location_t>	locations;

		while (++j < this->_countLines && this->_tokens[j].back() != "}"){
			if (this->_tokens[j].back() == "{"){
				location_t loc;
				if (!getLocation(j, loc))
					return false;
				locations.push_back(loc);
			}
			else if (!pushDirective(serv, j))
					return false;
		}
		j++;
		if (!checkLocations(locations))
			return false;
		serv.set_locations(locations);
		serv.fullBasicDirectives();
		this->_serversList.push_back(serv);
	}
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

//////////////////////////   Init   \\\\\\\\\\\\\\\\\\\\\\\\\\




//////////////////////////   Getters   \\\\\\\\\\\\\\\\\\\\\\\\\\

std::vector<Server>		ConfigParser::getServers(void) const{
	return this->_serversList;
}

int						ConfigParser::get_NumberOfServers(void) const{
	return this->_numberOfServers;
}

//////////////////////////   Getters   \\\\\\\\\\\\\\\\\\\\\\\\\\




//////////////////////////   Exceptions   \\\\\\\\\\\\\\\\\\\\\\\\\\

const char *ConfigParser::ConfigErrorException::what() const throw(){
	return "\033[31mConfiguration error\033[0m";
}

//////////////////////////   Exceptions   \\\\\\\\\\\\\\\\\\\\\\\\\\
