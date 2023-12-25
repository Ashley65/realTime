//
// Created by NIgel work on 23/12/2023.
//

#include "../../include/server/serverSocket.h"
#include <asio.hpp>
#include <thread>


void serverSocket::start() {
    // start the server and begin listening for incoming client connections
    std::cout << "Server starting" << std::endl;
    // open the socket
    open();
    // accept a new client connection
    accept();
    // start the io_service in a new thread
    std:: thread io_service_thread([this] { io_service_.run(); });
    io_service_thread.detach();
    std::cout << "Server started" << std::endl;







}

void serverSocket::stop() {
    // stop the server and close all active connections

}
void serverSocket::accept() {
    // accept an incoming client connection
    serverSocket *new_connection;
    new_connection = new serverSocket(io_service_, "", "");

    // Asynchronously accept a new connection
    acceptor_.async_accept(new_connection->socket_,
                          [this, new_connection](const asio::error_code &error) {
                              // The serverSocket object is automatically destroyed when the lambda exits,
                              // which closes the socket. This means that all shared_ptr references
                              // to the serverSocket object will disappear and the object will be destroyed
                              // *after* this handler ends. To avoid this, we must ensure that the
                              // serverSocket object is destroyed *before* the lambda function exits.
                              // We do this by creating another shared_ptr to the serverSocket
                              // from inside the lambda.
                              std::shared_ptr<serverSocket> new_connection_ptr(new_connection);
                              // handle the new connection request
                              handle_accept(new_connection_ptr.get(), error);
                              // accept another connection
                              accept();
                          });
}

void serverSocket::handle_accept(serverSocket *new_connection, const asio::error_code &error) {
    // handle a new client connection after it has been accepted

}

void serverSocket::handle_read(const asio::error_code &error, size_t bytes_transferred) {
    // handle incoming data from a client

}

void serverSocket::broadcast(const std::string &msg) {
    // send a message to all connected clients

}

void serverSocket::remove(serverSocket *client) {
    // remove a client from the list of active clients

}


