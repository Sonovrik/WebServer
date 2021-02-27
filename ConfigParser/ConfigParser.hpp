#ifndef CONFIGPARSER_HPPP
#define CONFIGPARSER_HPPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <array>

typedef struct		s_location{
	std::string		_name;
	std::map<std::string, std::string>	_directives;
}					location_t;


class ConfigParser{

private:
	// constants
	static std::array<std::string, 2> const _blocks;

	size_t			_numberOfServers;
	size_t			_numberOfLocations;
	std::string		_serverName;
	std::string		_ipToListen;
	std::string		_root;
	std::string		_maxBodySize;

	std::vector<std::string>	*_tokens;
	size_t						_countLines;
	std::vector<std::string>	_lines;

	// location_t		_locations[];
	std::string		_errorPage; // ???

	void			trimString(std::string &line);
	bool			checkBrackets(void);
	bool			fullTokens(void);
	bool			readConfig(const std::string &fileName);
	bool			getNumberServers(void);

	std::vector<std::string>	getTokens(std::vector<std::string>::iterator &it);

public:
	ConfigParser();
	~ConfigParser();

	bool			parseConfig(const std::string &fileName);
};


// replace
template<class T>
bool			isInArray(T first, T last, const std::string &line){
	size_t pos = 0;
	for (; first != last; first++){
		if ((pos = line.find(*first)) != std::string::npos)
				return true;
	}
	return false;
}


#endif