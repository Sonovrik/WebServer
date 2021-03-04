#ifndef CONFIGPARSER_HPPP
#define CONFIGPARSER_HPPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <array>
#include "../Server.hpp"

class ConfigParser{

private:
	// constants
	static std::array<std::string, 2> const		_blocks;
	static std::array<std::string, 5> const		_directives;
	static std::array<std::string, 8> const		_localDirectives;

	// servers
	int						_numberOfServers;
	std::vector<Server>		_serversList;

	// config inside lines
	std::vector<std::string>	*_tokens;
	std::vector<std::string>	_lines;
	size_t						_countLines;

	void			trimString(std::string &line);

	bool			readConfig(const std::string &fileName);

	bool			checkBrackets(void);
	bool			checkBlocks(void);
	bool			checkMainDirectives(void);
	bool			checkURIS(std::vector<location_t>	&locations) const;

	bool			fullTokens(void);
	bool			fullServers(void);

	bool			pushLocation(void);
	bool			pushDirective(Server	&serv, size_t index);

	bool						getLocation(size_t *index, location_t &location);
	std::vector<std::string>	getTokens(std::string str);

public:
	ConfigParser();
	~ConfigParser();

	bool					parseConfig(const std::string &fileName);
	std::vector<Server>		getServers(void) const;
	int						get_NumberOfServers(void) const;
};

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