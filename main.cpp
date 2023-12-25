#include <iostream>
#include  <asio.hpp>
#include "realTimeNetworking/include/server/serverSocket.h"

int main() {
    asio::io_service io_service;
    std::cout << "Starting server" << std::endl;

    serverSocket server(io_service, "127.0.0.1", "1234");
    std::cout << "Server started" << std::endl;
    server.start();
    std::string msg;
    while (msg != "exit") {
        std::cout << "Enter a message to send to all clients: ";
        std::getline(std::cin, msg);
        server.broadcast(msg);
    }
    server.stop();
    return 0;
}