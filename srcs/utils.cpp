#include "utils.hpp"

static	int		num(long long c)
{
	int		count;

	count = 0;
	if (c == 0)
		return (1);
	if (c < 0)
		count++;
	while (c != 0)
	{
		c = c / 10;
		count++;
	}
	return (count);
}

static char			*lltoa(long long c){
	int			count;
	char		*list;
	long long	number;

	number = c;
	count = num(number);
	list = new char[(count + 1)];
	if (number < 0)
	{
		list[0] = '-';
		number = -number;
	}
	list[count--] = '\0';
	while (number > 9)
	{
		list[count] = (char)((number % 10) + '0');
		number = number / 10;
		count--;
	}
	list[count] = (char)number + '0';
	return (list);
}

void	cleanString(std::string &str){
	for (size_t i = 0; i < str.size(); i++){
		str[i] = '\0';
	}
}

std::string		getFileText(std::string const &path){
	std::ifstream	in(path);
	std::string	tmp("");
	std::string	text("");
	while (getline(in, tmp)){
		text.append(tmp + '\n');
	}
	text.pop_back();
	return text;
}


void		trimString(std::string &line){
	size_t i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	line.erase(0, i);

	size_t len = line.length();
	if(len == 0)
		return ;
	i = len - 1;
	while (line[i] == ' ' || line[i] == '\t')
		i--;
	if (i < len)
		line.erase(i + 1, len);
}

std::string		to_string(long long number){
	char *tmp = lltoa(number);
	std::string ret(tmp);
	delete tmp;
	return ret;
}

bool location_tCompare(const location_t &x, const location_t &y){
	size_t	a = std::count(x._name.begin(), x._name.end(), '/');
	size_t	b = std::count(y._name.begin(), y._name.end(), '/');
	if (a > b)
		return true;
	if (a < b)
		return false;
	if (a == b){
		if (x._name > y._name)
			return true;
	}
	return false;
}

std::string const		ipToString(uint32_t addr){
	std::string ret;
	ret.append(to_string(addr & 0xff));
	ret.append(".");
	ret.append(to_string((addr >> 8) & 0xff));
	ret.append(".");
	ret.append(to_string((addr >> 16) & 0xff));
	ret.append(".");
	ret.append(to_string(addr >> 24));
	return ret;
}

std::string		createListingPage(std::vector<std::string> &files, Server const &serv,
										location_t const &loc, std::string path){
	std::string page;

	size_t pos = path.find(loc._directives.find("root")->second);
	path.erase(0, pos + loc._directives.find("root")->second.size() + 1);
	page.append("<html><head><title>Directory Listing</title></head>"
					"<body align=\"left\" style=\"font-size: 24px\">");
	for (int i = 1; i < files.size(); i++){
		page.append("<a href=\"http://");
		page.append(serv.get_ip() + ":" + serv.get_port() + loc._name + path + files[i]);
		page.append("\" target=\"CONTENT\">");
		page.append(to_string(i) + ".	");
		page.append(files[i]);
		page.append("</a><br>");
	}
	page.append("</body></html>");
	return page;
}

std::string		getListing(std::string const &path, Server const &serv, location_t const &loc){
	DIR *dir;
	struct dirent *diread;
	std::vector<std::string> files;

	if ((dir = opendir(path.c_str())) != nullptr) {
		while ((diread = readdir(dir)) != nullptr) {
			files.push_back(diread->d_name);
		}
		closedir (dir);
	}
	return createListingPage(files, serv, loc, path);
}
