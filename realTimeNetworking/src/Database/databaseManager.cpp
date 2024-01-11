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
