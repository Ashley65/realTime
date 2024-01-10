//
// Created by NIgel work on 26/12/2023.
//

#ifndef REALTIME_SERVERTCP_H
#define REALTIME_SERVERTCP_H

#include "../AbstractNetworking.h"
#include <iostream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "../encrypt/encryptAndDecrypt.h"


#pragma comment(lib, "ws2_32.lib") //Winsock Library
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

struct sockaddr_in;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


class ServerTCP : public AbstractNetworking {
private:
    SOCKET listenSocket;
    std::map<SOCKET, SSL*, std::less<>> clientSockets;

    std::mutex clientSocketsMutex;

    cryptoHandler cryptoHandler_;
public:
    ServerTCP();
    ~ServerTCP() override;

    void start() override;

    int receive(char* buffer, int bufferSize) override;

    int send(const char* message) override;


    void close() override;

private:
    static SOCKET initializeServerSocket();

    void handleNewConnection(SSL_CTX* sslContext);

    void handleClientData(int clientSocket, SSL_CTX* sslContext);

    void logChatData(const std::string& sender, const std::string& receiver, const std::string& message);

    void removeClient(int clientId);

    int receivedMessage{};
};



#endif //REALTIME_SERVERTCP_H
