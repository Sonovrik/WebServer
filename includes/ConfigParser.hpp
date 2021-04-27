#ifndef CONFIGPARSER_HPPP
#define CONFIGPARSER_HPPP

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include "Server.hpp"
#include <sys/stat.h>

class ConfigParser{

private:

	int						_numberOfServers;
	std::vector<Server>		_serversList;
	std::vector<std::string>	*_tokens;
	std::vector<std::string>	_lines;
	size_t						_countLines;

	bool			readConfig(const std::string &fileName);

	bool			checkBrackets(void) const;
	bool			checkBlocks(void);
	bool			checkMainDirectives(void) const;
	bool			checkURIS(std::vector<location_t>	&locations) const;
	bool			checkLocations(std::vector<location_t>	&locations) const;

	bool			fullTokens(void);
	bool			fullServers(void);
	bool			fullLocation(location_t &location);

	bool			pushLocation(void);
	bool			pushDirective(Server &serv, size_t index);

	bool						getLocation(size_t &index, location_t &location);
	std::vector<std::string>	getTokens(std::string str);

public:

	// Coplin
	ConfigParser();
	~ConfigParser();


	// parser
	bool					parseConfig(const std::string &fileName);
	
	// getters
	std::vector<Server>		getServers(void) const;
	int						get_NumberOfServers(void) const;


	// exceptions
	class ConfigErrorException: public std::exception{
		const char *what() const throw();
	};
};

#endif