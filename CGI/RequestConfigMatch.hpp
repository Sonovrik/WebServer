//
// Created by Kory Moaning on 3/20/21.
//

#ifndef REQUESTCONFIGMATCH_HPP
#define REQUESTCONFIGMATCH_HPP
#include "CGI.hpp"
#include "../utils.hpp"
#include "../Response.hpp"
#include "../Client/Client.hpp"

static const int B64index[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
	56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3,  4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
	0, 0, 0, 63, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

size_t						countChar(const std::string& str, char c);
std::vector<std::string>	splitString(std::string method);
int							compareLocation(std::string &uri, location_t loc, std::string &res);
std::string					getLocation(std::string &uri, Server &ser, int &pos);
void						checkIndex(std::string &ret, Server const &serv, location_t &location);
std::string					getPath(std::string &uri, int &loc, Request &req, const Server &ser);
void						compareHostName(const std::string& hostName, const std::string& ip, const std::string& servName);
void						comparePort(const std::string& port, const std::string& servPort);
int							checkHost(int pos, std::string &uri, Server &ser);
void						checkBodySize(Server &ser, int locIndex, Request &req);
bool						getWhere(std::map<std::string, std::string> dir, Request &req);
void						checkConf(Server &ser, int locIndex, Request &req, Client &client);
int							RequestConfigMatch(Client &client, Server &ser);
void						setErrorCode(const std::string& str, Client &client);

#endif

