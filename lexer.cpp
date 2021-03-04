#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

//	rules
// каждая директива оканчивается ;
// после скобок ничего не должно быть как и после ;
// все что не входит в ключи дает ошибку
// кодировка только аски
// двоеточия запрещены
// если скобка не закрыта это неверно


const std::string _keys[] = {"listen", "server_name",
		"location", "server", "root", "autoindex", "max_buffer_size"};

void		trim_string(std::string &line){
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

bool		check_invalid_chars(std::string &line){
	for (int i = 0; i < line.length(); i++){
		if (isascii(line[i]) == 0 || line[i] == '\'' || line[i] == '\"')
			return false;
	}
	return true;
}

void		parse_tokens(std::vector<std::string> &tokens, std::string &line){
	std::string tmp("");
	for(int i = 0; i < line.size(); i++){
		if (line[i] == '{' || line[i] == '}' || line[i] == ';' || line[i] == '\t' || line[i] == ' '){
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
	if (line.size() != 0)
		tokens.push_back("\n");
}

std::vector<std::string>	start_lexer(std::string line){
	std::vector<std::string> tokens;
	std::string tmp;

	size_t pos = 0;
	while ((pos = line.find('\n')) != std::string::npos){
		tmp = line.substr(0, pos);
		trim_string(tmp);
		size_t len = tmp.length();
		line.erase(0, pos + 1);
		size_t lenFirst = tokens.size();
		parse_tokens(tokens, tmp);
		size_t lenSecond = tokens.size();
	}
	return tokens;
}
 
bool		validate_config(std::vector<std::string> tokens){
	int a = 0, b = 0, c = 0;
	std::vector<std::string>::iterator it = tokens.begin();
	while (it != tokens.end()){
		c = 0;
		size_t len = 0;
		while(it != tokens.end() && *it != "\n"){
			len++;
			if (check_invalid_chars(*it) == 0)
				return false;
			if (*it == "{")
				a++;
			else if (*it == "}")
				b++;
			else if (*it == ";")
				c++;
			if (it != tokens.end() && (*it == "{" || *it == "}") && *(it + 1) != "\n")
				return false;
			if (it != tokens.end() && *it == ";" && *(it + 1) != "\n")
				return false;
			it++;
		}
		if (len == 1 && *(it - 1) == ";")
			return false;
		if (it != tokens.end() && len < 3 && *(it - 1) != "{" && *(it - 1) != "}" && *(it - 1) != ";")
			return false;
		if (c > 1)
			return false;
		if (it != tokens.end() && *it == "\n" && *(it - 1) != ";" && *(it - 1) != "{" && *(it - 1) != "}")
			return false;
		it++;
	}
	if (a != b)
		return false;
	return true;
}

bool		isInArray(std::string line, size_t len, const std::string _keys[]){
	for (int i = 0; i < len; i++){
		if (_keys[i] == line)
			return true;
	}
	return false;
}

bool		check_parametrs(std::vector<std::string> tokens){
	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator end;

	end = tokens.end();
	it = tokens.begin();
	if (*it != "server" || ((it + 1) != end && *(it + 1) != "{"))
		return false;
	size_t	c = 1;
	it += 3;
	for (;it < end; it++){
		if (*it == "{")
			c++;
		else if (*it == "}")
			c--;
		else{
			if (isInArray(*it, 7, _keys) == false){
				std::cout << *it << std::endl;
				return false;
			}
		}
		while (it != end && *it != "\n"){
			if (*it == "{")
				c++;
			else if (*it == "}")
				c--;
			it++;
		}
		if (((it + 2) != end) && c == 0)
			return false;
	}
	return true;
}

int main()
{
	std::string line;
	std::string out("");
	std::ifstream in("serv.conf");
	if (in.is_open())
	{
		while (getline(in, line))
		{
			out += line + "\n";
		}
	}
	in.close();
	std::vector<std::string> tokens = start_lexer(out); 
	if (validate_config(tokens) != true)
		std::cout << "False" << std::endl;
	// for (std::vector<std::string>::iterator it = tokens.begin(); it < tokens.end(); it++)
	// 	std::cout << *it << std::endl;
	if (check_parametrs(tokens) != true)
		std::cout << "False" << std::endl;
		// return -1;
	// std::map<std::string, std::string> directives = push_directives(tokens);
	std::vector<std::string>::iterator it;
	
	std::cout << "End of program" << std::endl;
	std::vector<std::map<std::string, std::vector<std::string> > > fgd;
	return 0;
}