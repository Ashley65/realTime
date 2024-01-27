//
// Created by NIgel work on 10/01/2024.
//

#include "../../include/Database/databaseManager.h"
#include <iostream>

bool databaseManager::open(const char* dbName) {
    if (sqlite3_open(dbName, &db_) != SQLITE_OK) {
        std::runtime_error give_me_a_name("Failed to open database.");
        return false;
    }
    return true;
}

void databaseManager::closeDatabase(){
    if (db_){
        sqlite3_close(db_);
        db_ = nullptr;

    }
}

// This function is used to execute SQL statements that don't return any data
int databaseManager::executeStatement(const char* sqlStatement) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db_, sqlStatement, nullptr, nullptr, &errorMessage);
    if (result != SQLITE_OK) {
        std::cerr <<"SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
    return result;
}

sqlite3 * databaseManager::getDatabase() {
    return db_;
}

databaseManager* databaseManager::createConnection() {
    return new databaseManager();
}


// ConnectionPool

databaseManager* ConnectionPool::getConnection(){
    unique_lock<mutex> lock(poolMutex);
    cv.wait(lock, [this](){return !connections.empty();});

    databaseManager* connection = connections.front();
    connections.pop();
    return connection;

}

void ConnectionPool::returnConnection(databaseManager* connection){
    lock_guard<mutex> lock(poolMutex);
    connections.push(connection);
    cv.notify_one();
}
