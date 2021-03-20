cmake_minimum_required(VERSION 3.17)
project(WebServer)

set(CMAKE_CXX_STANDARD 98)

add_executable(WebServer CGI/CGI.cpp CGI/CGI.hpp CGI/mainCGI.cpp
        ConfigParser/ConfigParser.cpp ConfigParser/ConfigParser.hpp
        Server.hpp server.cpp
        Request/Request.cpp Request/Request.hpp
        Client/Client.cpp Client/Client.hpp
        utils.cpp utils.hpp
        Response.cpp Response.hpp errors.cpp)

#CGI/CGI.cpp CGI/mainCGI.cpp ConfigParser/ConfigParser.cpp server.cpp Request/Request.cpp Client/Client.cpp utils.cpp