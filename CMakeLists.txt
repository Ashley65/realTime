cmake_minimum_required(VERSION 3.26)
project(realTime)

set(CMAKE_CXX_STANDARD 23)

add_executable(realTime main.cpp
        realTimeNetworking/include/abstractSocket/asbtractSocket.h
        realTimeNetworking/include/server/serverSocket.h
        realTimeNetworking/src/server/serverSocket.cpp)

target_link_libraries(realTime pthread)
include_directories(E:/realTime/asio-master/asio/include)

target_link_libraries(realTime wsock32 ws2_32)