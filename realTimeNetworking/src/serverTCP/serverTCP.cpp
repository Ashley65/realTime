
#include "../../include/serverTCP/serverTCP.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sqlite3.h>


// serverTCP.cpp

ServerTCP::ServerTCP() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        throw std::runtime_error("Failed to initialize Winsock.");
        exit (EXIT_FAILURE);
    }

    // initialize OpenSSL
    SSL_load_error_strings();
    SSL_library_init();

    listenSocket = initializeServerSocket();
}

ServerTCP::~ServerTCP() {
    close();
    WSACleanup(); // Cleanup Winsock
    ERR_free_strings();
    EVP_cleanup();
}

void ServerTCP::start() {
    //setup SSL context and configure server to use SSL
    SSL_CTX* sslContext = SSL_CTX_new(TLS_server_method());
    if (!sslContext) {
        throw std::runtime_error("Failed to create SSL context.");
        exit (EXIT_FAILURE);
    }
    if (SSL_CTX_use_certificate_file(sslContext, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(sslContext, "server.key", SSL_FILETYPE_PEM) <= 0){
        throw std::runtime_error("Failed to load certificate and/or private key file.");
        exit (EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to listen for incoming connections.");
        exit (EXIT_FAILURE);
    }
    //display server info
    std::cout << "Server listening on port " << DEFAULT_PORT << std::endl;

    // Enter a loop to handle incoming connections and data
    while (true) {
        // clear the socket set
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listenSocket, &readfds);

        // Add client sockets to the set
        for (auto const& pair : clientSockets) {
            FD_SET(pair.first, &readfds);
        }

        // Wait for activity on any of the sockets
        select(0, &readfds, nullptr, nullptr, nullptr);

        // check for incoming connections
        if (FD_ISSET(listenSocket, &readfds)){
            handleNewConnection(sslContext);
        }

        // Check for incoming data from clients
        for (auto& clientSocket : clientSockets) {
            if (FD_ISSET(clientSocket.second, &readfds)) {
                handleClientData(clientSocket.first, sslContext);
            }
        }
    }
}

int ServerTCP::receive(char* buffer, int bufferSize) {
    // Implement receive method here
    // ...
    return 0; // Placeholder, replace with actual implementation
}

int ServerTCP::send(const char* message) {
    // Implement send method here
    // ...
    return 0; // Placeholder, replace with actual implementation
}

void ServerTCP::close() {
    //claose all client sockets
    for (auto const& pair : clientSockets) {
        closesocket(pair.first);
    }

    // Close the server socket
    closesocket(listenSocket);
}

SOCKET ServerTCP::initializeServerSocket() {
    // Create a socket for the server to listen for client connections
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create server socket.");
        exit (EXIT_FAILURE);
    }

    // prepare the sockaddr_in structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(std::stoi(DEFAULT_PORT));

    // Bind the server socket
    if (::bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to bind server socket.");
        closesocket(serverSocket);
        exit (EXIT_FAILURE);
    }

    return serverSocket;
}

void ServerTCP::handleNewConnection(SSL_CTX* sslContext) {
    sockaddr_in clientAddress;
    int addrlen = sizeof(clientAddress);

    // Accept a new client connection
    SOCKET newSocket  = accept(listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &addrlen);
    if (newSocket == INVALID_SOCKET) {
        throw std::runtime_error("Failed to accept client connection.");
        return;

    }

    //setup SSL connection for new client socket
    SSL* ssl = SSL_new(sslContext);
    SSL_set_fd(ssl, newSocket);

    if (SSL_accept(ssl) <= 0) {
        throw std::runtime_error("SSL handshake failed.");
        SSL_free(ssl);
        closesocket(newSocket);
        return;
    }

    // Add the new client socket to the map
    clientSockets.insert({newSocket, ssl});

    std::cout << "New connection, socket fd is " << newSocket
              << ", ip is " << inet_ntoa(clientAddress.sin_addr)
              << ", port is " << ntohs(clientAddress.sin_port) << std::endl;

}

void ServerTCP::handleClientData(int clientSocket, SSL_CTX* sslContext) {
    SSL* ssl = reinterpret_cast<SSL *>(clientSockets[clientSocket]);
    // Implement handling of data from a client here

    // ...
}
void ServerTCP::logChatData(const std::string& sender, const std::string& receiver, const std::string& message) {
    // Implement logging of chat data into a database (e.g., SQLite) here
    // You can use the SQLite C API to interact with the database.
    // Example:
    // sqlite3* db;
    // sqlite3_open("chat_log.db", &db);
    // sqlite3_exec(db, "INSERT INTO chat_log (sender, receiver, message) VALUES ('user1', 'user2', 'Hello!');", 0, 0, 0);
    // sqlite3_close(db);
    // Placeholder for logging chat data
    logChatData("user1", "user2", receivedMessage);
}

void ServerTCP::removeClient(int clientId) {
    // Implement removing a client from the map here
    // ...
}