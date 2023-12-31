//
// Created by NIgel work on 26/12/2023.
//

#ifndef REALTIME_CLIENTTCP_H
#define REALTIME_CLIENTTCP_H
#include "../AbstractNetworking.h"
#include <windows.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


class ClientTCP : public AbstractNetworking {
private:
    SOCKET clientSocket;

public:
    ClientTCP(const char* serverIP, const char* serverPort);
    ~ClientTCP() override;

    void start() override;

    int receive(char* buffer, int bufferSize) override;

    int send(const char* message) override;

    void close() override;
};


#endif //REALTIME_CLIENTTCP_H
