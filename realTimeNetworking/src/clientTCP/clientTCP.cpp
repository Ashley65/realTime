//
// Created by NIgel work on 26/12/2023.
//

#include "../../include/clientTCP/clientTCP.h"


ClientTCP::ClientTCP(const char* serverIP, const char* serverPort) {
    // Initialize Winsock
    // ...

    // Initialize client socket
    // ...
}

ClientTCP::~ClientTCP() {
    close();
    WSACleanup(); // Cleanup Winsock
}

void ClientTCP::start() {
    // Implement connection setup here
    // ...
}

int ClientTCP::receive(char* buffer, int bufferSize) {
    // Implement receive method here
    // ...
    return 0; // Placeholder, replace with actual implementation
}

int ClientTCP::send(const char* message) {
    // Implement send method here
    // ...
    return 0; // Placeholder, replace with actual implementation
}

void ClientTCP::close() {
    // Close the client socket
    // ...
}
