cmake_minimum_required(VERSION 3.17)
project(ConfigParser)

set(CMAKE_CXX_STANDARD 98)

include_directories(.)

add_executable(ConfigParser
        ConfigParser.cpp
        ConfigParser.hpp)
