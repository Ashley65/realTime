//
// Created by NIgel work on 26/12/2023.
//

#ifndef REALTIME_CLIENTTCP_H
#define REALTIME_CLIENTTCP_H
#include "../AbstractNetworking.h"
#include <windows.h>
#include <winsock2.h>
#include "../encrypt/encryptAndDecrypt.h"
#include <openssl/evp.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"


class ClientTCP : public AbstractNetworking {
private:
    SOCKET clientSocket;
    cryptoHandler cryptoHandler_; // Add a cryptoHandler object as a member variable


public:
    ClientTCP(const char* serverIP, const char* serverPort) :
    cryptoHandler_(32, 16){
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
            throw std::runtime_error("Failed to initialize Winsock.");
        }
        std::cout << "Winsock initialized." << std::endl;

        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create client socket.");
        }
        std::cout << "Client socket created." << std::endl;

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(27016); // Replace with your server port
        if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0) { // Replace with your server IP
            throw std::runtime_error("Failed to setup server address.");
        }
        std::cout << "Server address setup." << std::endl;

        cryptoHandler_.generateKeyPair();

        std::cout << "Public key: " <<  std::endl;

        // Connect to the server#
        if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
            int error = WSAGetLastError();
            throw std::runtime_error("Failed to connect to the server. Error code: " + std::to_string(error));
        }
        std::cout << "Connected to the server." << std::endl;
    }
    ~ClientTCP() override{
        closesocket(clientSocket);
        WSACleanup();
    }

    void start() override;

    int receive(char* buffer, int bufferSize) override;

    int send(const char* message) override;


    void close() override;
};


#endif //REALTIME_CLIENTTCP_H
