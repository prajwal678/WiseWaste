#pragma once

#include <memory>
#include <sqlite3.h>
#include <string>

class Database {
public:
    Database();
    ~Database();

    bool connect();
    sqlite3* getDb() const;
    
    bool executeQuery(const std::string& query);
    bool executeQueryWithCallback(const std::string& query, 
                                int (*callback)(void*, int, char**, char**), 
                                void* data);

private:
    sqlite3* db;
    static const std::string DB_FILE;
}; 