//
// Created by Kory Moaning on 3/20/21.
//

#ifndef REQUESTCONFIGMATCH_HPP
#define REQUESTCONFIGMATCH_HPP
#include "CGI.hpp"
#include "../utils.hpp"
#include "../Response.hpp"
#include "../Client/Client.hpp"

size_t						countChar(const std::string& str, char c);
std::vector<std::string>	splitString(std::string method);
int							compareLocation(std::string &uri, location_t loc, std::string &res);
std::string					getLocation(std::string &uri, Server &ser, int &pos);
void						checkIndex(std::string &ret, const Server &ser, int loc);
std::string					getPath(std::string &uri, int &loc, Request &req, const Server &ser);
void						compareHostName(const std::string& hostName, const std::string& ip, const std::string& servName);
void						comparePort(const std::string& port, const std::string& servPort);
int							checkHost(int pos, std::string &uri, Server &ser);
void						checkBodySize(Server &ser, int locIndex, Request &req);
void						setWhere(Server &ser, int locIndex, Request &req, Client &client);
void						checkConf(Server &ser, int locIndex, Request &req, Client &client);
int							RequestConfigMatch(Client &client, Server &ser);



#endif

