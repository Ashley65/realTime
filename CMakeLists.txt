cmake_minimum_required(VERSION 3.26)
project(realTime)

set(CMAKE_CXX_STANDARD 23)

add_executable(realTime main.cpp
        realTimeNetworking/include/abstractSocket/asbtractSocket.h)

include_directories(C:/libraries/boost_1_84_0)
link_directories(C:/libraries/boost_1_84_0/stage/lib)