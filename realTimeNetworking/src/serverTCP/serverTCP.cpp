#include "../../include/serverTCP/serverTCP.h"#
#include <iostream>
#include <thread>

#include <openssl/ssl.h>
#include <openssl/err.h>


ServerTCP::~ServerTCP() {
    terminate(); // Set the termination flag to gracefully shut down the server
    close(); // Close the server socket

    SSL_CTX_free(sslContext);// Clean up the SSL context
    sslContext = nullptr;
    dbManager.closeDatabase();// Close the database connection
    WSACleanup(); // Cleanup Winsock
    ERR_free_strings();
    EVP_cleanup();
}

void ServerTCP::start() {
    // Listen for incoming connections
    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::runtime_error give_me_a_name("Failed to listen for incoming connections.");
        exit (EXIT_FAILURE);
    }
    //display server info
    std::cout << "Server listening on port: " << DEFAULT_PORT << std::endl;


    // Enter a loop to handle incoming connections and data
    while (!shouldTerminate) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept new client connection." << std::endl;
            continue;
        }
        std::cout << "New client connection accepted." << std::endl;
        threadPool.enqueue([this, clientSocket](){ this->handleClient(clientSocket, sslContext); }); // add a new task to the thread pool


    }




}

int ServerTCP::receive(char* buffer, int bufferSize) {
    // Receiving encrypted data from a client

    std::cout << "Receiving data from the client." << std::endl;
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

    std::cout << "Server socket created." << std::endl;

    // Bind the server socket
    if (::bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::runtime_error give_me_a_name("Failed to bind server socket.");
        closesocket(serverSocket);
        exit (EXIT_FAILURE);
    }
    std::cout << "Server socket bound." << std::endl;

    return serverSocket;
}

void ServerTCP::handleNewConnection(SSL_CTX* sslContext) {
    sockaddr_in clientAddress{};
    int addrlen = sizeof(clientAddress);



    // Accept a new client connection
    SOCKET newSocket  = accept(listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &addrlen);
    if (newSocket == INVALID_SOCKET) {
        throw std::runtime_error("Failed to accept new client connection.");

    }
    std::cout << "New client connection accepted." << std::endl;

    // set a timeout for the new socket
    DWORD timeout = 5000;
    if (setsockopt(newSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0 ||
        setsockopt(newSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set timeout for client socket." << std::endl;
        closesocket(newSocket);
        return;
    }




    //setup SSL connection for new client socket
    SSL* ssl = SSL_new(sslContext);
    SSL_set_fd(ssl, newSocket);

    if (SSL_accept(ssl) <= 0) {
        std::runtime_error give_me_a_name("SSL handshake failed.");

        closesocket(newSocket);
        return;
    }
    if (clientSockets.find(newSocket) == clientSockets.end()) {
        // Add the new client socket to the map
        clientSockets.insert(std::pair<SOCKET, SSL*>(newSocket, ssl));


    }

    // Add the new client socket to the map
    clientSockets.insert(std::pair<SOCKET, SSL*>(newSocket, ssl));




    // Print a message indicating a new client has connected
    std::cout << "New client connected, socket fd is " << newSocket
              << ", ip is " << inet_ntoa(clientAddress.sin_addr)
              << ", port is " << ntohs(clientAddress.sin_port) << std::endl;





    // create a new thread to handle the new connection
    std::thread clientThread(&ServerTCP::handleClient, this, newSocket, sslContext);
    clientThread.detach(); // detach the thread so that it runs in the background

}

void ServerTCP::handleClientData(int clientSocket, SSL_CTX* sslContext) {
    // Buffer to store the received data
    char buffer[DEFAULT_BUFLEN];

    // sending a welcome message to the client after connection using the send method
    send("Welcome to the server!");

    // Receiving encrypted data from a client

    SSL *ssl = (clientSockets[clientSocket]);

    // Receive encrypted data from the client
    int receivedBytes = SSL_read(ssl,buffer,DEFAULT_BUFLEN);


    while(true){


        // Decrypt the received data
        if (receivedBytes <= 0){
           int error = SSL_get_error(ssl, receivedBytes);
           if (error == SSL_ERROR_ZERO_RETURN){
               std::cout << "client disconnected" << std::endl;
               closesocket(clientSocket);
               clientSockets.erase(clientSocket);
               SSL_free(ssl);// free the ssl object
           }else if (error == SSL_ERROR_SYSCALL){
               std::cout << "client disconnected" << std::endl;
               closesocket(clientSocket);
               clientSockets.erase(clientSocket);
               SSL_free(ssl);// free the ssl object
           }else{
               std::cout << "error receiving data" << std::endl;
               SSL_free(ssl);// free the ssl object
           }
            break; // break out of the loop on error or client disconnect

        }else{
            // receiving data from the client using the receive method
            std::vector<unsigned char> encryptedData(buffer, buffer + receivedBytes);
            std::vector<unsigned char> decryptedData;

            cryptoHandler_.performCipherOperation(encryptedData, decryptedData, false);

            //process and use the decrypted data as needed also log the data
            std::string decryptedString(decryptedData.begin(), decryptedData.end());
            std::cout << "Received message: " << decryptedString << std::endl;
            logChatData("user1", "user2", decryptedString);
        }

    }
    //


}

[[noreturn]] void ServerTCP::handleClient(SOCKET clientSocket, SSL_CTX *sslContext) {
    // Implement handling of a client here
    while (true){
        std::shared_lock<std::shared_mutex> lock(clientSocketsMutex); // acquire a shared lock
        handleClientData(clientSocket, sslContext);

    }



}
void ServerTCP::terminate() {
    shouldTerminate = true;
}

void ServerTCP::logChatData(const std::string& sender, const std::string& receiver, const std::string& message) {
    // Implement logging of chat data into a database (e.g., SQLite) here

    // create a table for recoding chat data if it does not exist
    dbManager.executeStatement("CREATE TABLE IF NOT EXISTS chatData (id INTEGER PRIMARY KEY, sender TEXT NOT NULL, receiver TEXT NOT NULL, message TEXT NOT NULL, messageTime DATETIME);");

    // insert the chat data into the table
    std::string sqlStatement = "INSERT INTO chatData (sender, receiver, message, messageTime) VALUES ('" + sender + "', '" + receiver + "', '" + message + "', datetime('now'));";
    dbManager.executeStatement(sqlStatement.c_str());

      // query the database
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(dbManager.getDatabase(), "SELECT * FROM chatData;", -1, &stmt, nullptr) != SQLITE_OK){
        std::cerr << "Failed to prepare statement." << std::endl;
        return;
    }

    // print the results
    while (sqlite3_step(stmt) == SQLITE_ROW){
        std::cout << "id: " << sqlite3_column_int(stmt, 0) << ", sender: " << sqlite3_column_text(stmt, 1) << ", receiver: " << sqlite3_column_text(stmt, 2) << ", message: " << sqlite3_column_text(stmt, 3) << ", messageTime: " << sqlite3_column_text(stmt, 4) << std::endl;
    }
    // remove the prepared statement from memory
    sqlite3_finalize(stmt);


}

void ServerTCP::removeClient(int clientId) {
    std::unique_lock<std::shared_mutex> lock(clientSocketsMutex); // acquire a unique lock
    closesocket(clientId);
    clientSockets.erase(clientId);
    SSL_free(clientSockets[clientId]);// free the ssl object
}

ServerTCP::ServerTCP() :shouldTerminate(false), dbManager(), pool([this](){ return this->dbManager.createConnection(); }, 5), threadPool(5){
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        std::runtime_error give_me_a_name(&"Failed to initialize Winsock." [ WSAGetLastError()]);
        exit (EXIT_FAILURE);
    }
    // initialize DatabaseManage
    if (!dbManager.open("RealChat.db")){
        std::cerr << "Failed to open the database." << std::endl;
        exit (EXIT_FAILURE);
    }
    std::cout << "Opened the database successfully." << std::endl;

    // initialize OpenSSL
    SSL_load_error_strings();
    SSL_library_init();

    // initialize the cryptoHandler
    cryptoHandler_ = cryptoHandler(32, 16);
    cryptoHandler_.generateKeyPair();

    sslContext = SSL_CTX_new(TLS_server_method());
    if (!sslContext) {
        std::cerr<<"Failed to create SSL context."<<std::endl;
        exit (EXIT_FAILURE);
    }
    if (SSL_CTX_use_certificate_file(sslContext, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(sslContext, "server.key", SSL_FILETYPE_PEM) <= 0){
        std::cerr<<"Failed to load certificate and/or private key file."<<std::endl;
        exit (EXIT_FAILURE);
    }
    if (!SSL_CTX_check_private_key(sslContext)) {
        std::cerr<<"Private key does not match the public certificate."<<std::endl;
        exit (EXIT_FAILURE);
    }
    std::cout << "SSL context created." << std::endl;


    listenSocket = initializeServerSocket();
}

