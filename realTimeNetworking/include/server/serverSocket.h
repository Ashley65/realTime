//
// Created by NIgel work on 23/12/2023.
//

#ifndef REALTIME_SERVERSOCKET_H
#define REALTIME_SERVERSOCKET_H

#include <iostream>
#include "../abstractSocket/asbtractSocket.h"
#include <set>
#include <asio.hpp>



class serverSocket: public asbtractSocket {
public:
    serverSocket(asio::io_service& io_service, const std::string& host, const std::string& port)
            : asbtractSocket(io_service, host, port), acceptor_(io_service), io_service_(io_service) {
    }
    void start();
    void stop();
    void accept();
    void handle_accept(serverSocket* new_connection, const asio::error_code& error);
    void handle_read(const asio::error_code& error, size_t bytes_transferred);
    void broadcast(const std::string& msg);
    void remove(serverSocket* client);

private:


    std::set<serverSocket*> clients_;
    enum { max_length = 1024 };
    char data_[max_length]{};

    asio::ip::tcp::acceptor acceptor_;
    asio::io_service &io_service_;
};


#endif //REALTIME_SERVERSOCKET_H
