//
// Created by NIgel work on 26/12/2023.
//

#ifndef REALTIME_ABSTRACTNETWORKING_H
#define REALTIME_ABSTRACTNETWORKING_H

#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
struct sockaddr_in;


class AbstractNetworking {
public:
    virtual ~AbstractNetworking() = default;

    virtual void start() = 0;

    virtual int receive(char* buffer, int bufferSize) = 0;

    virtual int send(const char* message) = 0;

    virtual void close() = 0;
};




#endif //REALTIME_ABSTRACTNETWORKING_H
