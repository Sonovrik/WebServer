cmake_minimum_required(VERSION 3.17)
project(WebServer)

set(CMAKE_CXX_STANDARD 98)

add_executable(WebServer CGI/CGI.cpp CGI/CGI.hpp CGI/mainCGI.cpp
        ConfigParser/ConfigParser.cpp ConfigParser/ConfigParser.hpp
        Server.hpp server.cpp
        parser/parser.cpp parser/Request.cpp parser/Request.hpp)

#CGI.cpp mainCGI.cpp ../ConfigParser/ConfigParser.cpp ../server.cpp ../parser/parser.cpp ../parser/Request.cpp