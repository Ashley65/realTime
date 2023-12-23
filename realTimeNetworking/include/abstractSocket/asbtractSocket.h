//
// Created by NIgel work on 23/12/2023.
//

#ifndef REALTIME_ASBTRACTSOCKET_H
#define REALTIME_ASBTRACTSOCKET_H


#include <string>
#include <boost/asio.hpp>




class asbtractSocket {
public:
    // the constructor will open the socket
    asbtractSocket(boost::asio::io_service& io_service, const std::string& host, const std::string& port)
        :socket_(io_service){
            boost::asio::ip::tcp::resolver resolver(io_service);
            boost::asio::ip::tcp::resolver::query query(host, port);
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            boost::asio::ip::tcp::resolver::iterator end;
            boost::system::error_code error = boost::asio::error::host_not_found;
            while (error && endpoint_iterator != end){
                socket_.close();
                socket_.connect(*endpoint_iterator++, error);
            }
            if (error)
                throw boost::system::system_error(error);

    }
    // the destructor will close the socket
    virtual ~asbtractSocket() {
        close();
    }
    // send data to the socket
    void open() {
        socket_.open(boost::asio::ip::tcp::v4());
    }
    // close the socket
    void close() {
        if (socket_.is_open())
            socket_.close();
    }
    // this function will send data to the socket and return the number of bytes sent
    virtual std::string receive() {
        boost::asio::streambuf buf;
        boost::asio::read_until(socket_, buf, "\n");
        std::string data = boost::asio::buffer_cast<const char*>(buf.data());
        return data;
    }
    virtual size_t send(const std::string& data) {
        return socket_.write_some(boost::asio::buffer(data, data.size()));
    }




protected:
    boost::asio::ip::tcp::socket socket_;
};


#endif //REALTIME_ASBTRACTSOCKET_H
