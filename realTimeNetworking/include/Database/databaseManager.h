//
// Created by NIgel work on 10/01/2024.
//

#ifndef REALTIME_DATABASEMANAGER_H
#define REALTIME_DATABASEMANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>


class databaseManager {
private:
    sqlite3* db_;
public:
    databaseManager(): db_(nullptr){};
    ~databaseManager() {closeDatabase();};

    bool open(const char* dbName);

    int executeStatement(const char* sqlStatement);

    void getTableNames(std::vector<std::string>& tableNames);
    sqlite3 * getDatabase();
    void closeDatabase();


};


#endif //REALTIME_DATABASEMANAGER_H
