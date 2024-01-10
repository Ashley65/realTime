
#include "../../include/serverTCP/serverTCP.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sqlite3.h>


// serverTCP.cpp

ServerTCP::ServerTCP() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        std::runtime_error give_me_a_name("Failed to initialize Winsock.");
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
        std::runtime_error("Failed to create SSL context.");
        exit (EXIT_FAILURE);
    }
    if (SSL_CTX_use_certificate_file(sslContext, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(sslContext, "server.key", SSL_FILETYPE_PEM) <= 0){
        std::runtime_error("Failed to load certificate and/or private key file.");
        exit (EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::runtime_error give_me_a_name("Failed to listen for incoming connections.");
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
    // Receiving encrypted data from a client
    SOCKET clientSocket = reinterpret_cast<SOCKET>(clientSockets[0]);
    SSL *ssl = (clientSockets[clientSocket]);

    // Receive encrypted data from the client
    int receivedBytes = SSL_read(ssl,buffer,bufferSize);

    // Decrypt the received data
    if (receivedBytes <= 0){
       int error = SSL_get_error(ssl, receivedBytes);
       if (error == SSL_ERROR_ZERO_RETURN){
           std::cout << "client disconnected" << std::endl;
           closesocket(clientSocket);
           clientSockets.erase(clientSocket);
       }else if (error == SSL_ERROR_SYSCALL){
           std::cout << "client disconnected" << std::endl;
           closesocket(clientSocket);
           clientSockets.erase(clientSocket);
       }else{
              std::cout << "error receiving data" << std::endl;
       }

    }else {
        //decrypt data using the cryptoHandler
        std::vector<unsigned char> encryptedData(buffer, buffer + receivedBytes);
        std::vector<unsigned char> decryptedData;

        cryptoHandler_.performCipherOperation(encryptedData, decryptedData, false);

        //process and use the decrypted data as needed also log the data
        std::string decryptedString(decryptedData.begin(), decryptedData.end());
        std::cout << "Received message: " << decryptedString << std::endl;
        logChatData("user1", "user2", decryptedString);
    }
    return receivedBytes;


}

int ServerTCP::send(const char* message) {
    // Implement send method here
    SOCKET clientSocket = 0;
    SSL* ssl = clientSockets[clientSocket];

    // Encrypt the message using the cryptoHandler
    std::vector<unsigned char> plainMessage(message, message + strlen(message));
    std::vector<unsigned char> encryptedData;
    cryptoHandler_.performCipherOperation(plainMessage, encryptedData, true);

    // Send the encrypted data to the client
    int sentBytes = SSL_write(ssl, reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
    if (sentBytes <= 0){
        std::cout << "error sending data" << std::endl;
    }
    return sentBytes;

}

void ServerTCP::close() {
    //close all client sockets
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
        std::runtime_error give_me_a_name("Failed to create server socket.");
        exit (EXIT_FAILURE);
    }

    // prepare the sockaddr_in structure
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(std::stoi(DEFAULT_PORT));

    // Bind the server socket
    if (::bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::runtime_error give_me_a_name("Failed to bind server socket.");
        closesocket(serverSocket);
        exit (EXIT_FAILURE);
    }

    return serverSocket;
}

void ServerTCP::handleNewConnection(SSL_CTX* sslContext) {
    sockaddr_in clientAddress{};
    int addrlen = sizeof(clientAddress);

    // Accept a new client connection
    SOCKET newSocket  = accept(listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &addrlen);
    if (newSocket == INVALID_SOCKET) {
        std::runtime_error give_me_a_name("Failed to accept client connection.");
        return;

    }

    //setup SSL connection for new client socket
    SSL* ssl = SSL_new(sslContext);
    SSL_set_fd(ssl, newSocket);

    if (SSL_accept(ssl) <= 0) {
        std::runtime_error give_me_a_name("SSL handshake failed.");
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
    SSL* ssl = clientSockets[clientSocket];

    char buffer[DEFAULT_BUFLEN];
    int receivedBytes = SSL_read(ssl, buffer, DEFAULT_BUFLEN);
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

}

void ServerTCP::removeClient(int clientId) {
    // Implement removing a client from the map here
    // ...
}