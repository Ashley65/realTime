//
// Created by NIgel work on 26/12/2023.
//

#include "../../include/clientTCP/clientTCP.h"




void ClientTCP::start() {
    // implement start method here
    std::cout << "Client started" << std::endl;



}

int ClientTCP::receive(char* buffer, int bufferSize) {
    int result = recv(clientSocket, buffer, bufferSize, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::vector<unsigned char> encryptedMessage(buffer, buffer + result);
    std::vector<unsigned char> decryptedMessage;
    cryptoHandler_.performCipherOperation(encryptedMessage, decryptedMessage, false);
    std::string decryptedString(decryptedMessage.begin(), decryptedMessage.end());
    std::cout << "Bytes received: " << decryptedString << std::endl;
    return 0;
}

int ClientTCP::send(const char* message) {
    std::vector<unsigned char> encryptedMessage;
    std::vector<unsigned char> decryptedMessage(message, message + strlen(message));
    cryptoHandler_.performCipherOperation(decryptedMessage, encryptedMessage, true);
    int result = ::send(clientSocket, reinterpret_cast<const char*>(encryptedMessage.data()), encryptedMessage.size(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Bytes sent: " << result << std::endl;
    return 0;
}

void ClientTCP::close() {
    int iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Client closed" << std::endl;


}

