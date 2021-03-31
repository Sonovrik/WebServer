cmake_minimum_required(VERSION 3.17)
project(WebServer)

set(CMAKE_CXX_STANDARD 98)

add_executable(WebServer startServer.cpp CGI/CGI.cpp CGI/CGI.hpp CGI/mainCGI.cpp
        ConfigParser/ConfigParser.cpp ConfigParser/ConfigParser.hpp
        Server.hpp Server.cpp
        Request/Request.cpp Request/Request.hpp
        Client/Client.cpp Client/Client.hpp
        utils.cpp utils.hpp MIMEtypes.cpp
        Response.cpp Response.hpp errors.cpp CGI/RequestConfigMatch.cpp CGI/RequestConfigMatch.hpp)
