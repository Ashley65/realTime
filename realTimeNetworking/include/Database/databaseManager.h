//
// Created by NIgel work on 10/01/2024.
//

#ifndef REALTIME_DATABASEMANAGER_H
#define REALTIME_DATABASEMANAGER_H

#include <string>
#include <vector>
#include <mutex>
#include <sqlite3.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

class databaseManager {
private:
    sqlite3* db_;

public:
    databaseManager(): db_(nullptr){};
    ~databaseManager() {closeDatabase();};

    databaseManager* createConnection();
    bool open(const char* dbName);
    int executeStatement(const char* sqlStatement);
    sqlite3 * getDatabase();
    void closeDatabase();


};

class ConnectionPool{
private:
    queue<databaseManager*> connections;
    mutex poolMutex;
    condition_variable cv;
    function<databaseManager*()> connectionFactory;

public:
    ConnectionPool(function<databaseManager*()> factory, size_t size): connectionFactory(factory){
        for (size_t i = 0; i < size; ++i) {
            connections.push(connectionFactory());
        }
    }

    ~ConnectionPool(){
        while (!connections.empty()){
            auto connection = connections.front();
            connections.pop();
        }
    }

    databaseManager* getConnection();

    void returnConnection(databaseManager* connection);


};


#endif //REALTIME_DATABASEMANAGER_H
