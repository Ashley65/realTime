//
// Created by NIgel work on 11/01/2024.
//
#include "databaseManager.h"
#include <iostream>

int main(){
    databaseManager dbManager;

    // Open the database
    if (!dbManager.open("test.db")){
        std::cerr << "Failed to open database." << std::endl;
        return -1;
    }

    // Create a table
    dbManager.executeStatement("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT NOT NULL);");

    // Insert some data
    dbManager.executeStatement("INSERT INTO test (name) VALUES ('Nigel');");
    dbManager.executeStatement("INSERT INTO test (name) VALUES ('John');");
    dbManager.executeStatement("INSERT INTO test (name) VALUES ('Mary');");

    // Query the database
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(dbManager.getDatabase(), "SELECT * FROM test;", -1, &stmt, nullptr) != SQLITE_OK){
        std::cerr << "Failed to prepare statement." << std::endl;
        return -1;
    }
    // Print the results
    while (sqlite3_step(stmt) == SQLITE_ROW){
        std::cout << "id: " << sqlite3_column_int(stmt, 0) << ", name: " << sqlite3_column_text(stmt, 1) << std::endl;
    }

    // Close the database
    dbManager.closeDatabase();

    return 0;
}
