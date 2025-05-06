#include "database.h"
#include <iostream>
#include <sqlite3.h>

const std::string Database::DB_FILE = "wisewaste.db";

Database::Database() : db(nullptr) {}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::connect() {
    int rc = sqlite3_open(DB_FILE.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* createTableSQL = 
        "CREATE TABLE IF NOT EXISTS waste_pickups ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "waste_type TEXT NOT NULL,"
        "pickup_location TEXT NOT NULL,"
        "pickup_datetime TEXT NOT NULL,"
        "status TEXT NOT NULL,"
        "user_name TEXT NOT NULL"
        ");";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

sqlite3* Database::getDb() const {
    return db;
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool Database::executeQueryWithCallback(const std::string& query, 
                                     int (*callback)(void*, int, char**, char**), 
                                     void* data) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, data, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}
