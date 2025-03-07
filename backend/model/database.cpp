#include <iostream>
#include <sqlite3.h>
#include <string>

class Database {
private:
  sqlite3 *db;
  bool connected;

public:
  Database() : db(nullptr), connected(false) {}

  bool connect(const std::string &dbName = "waste_management.db") {
    int rc = sqlite3_open(dbName.c_str(), &db);

    if (rc) {
      std::cerr << "Error opening database: " << sqlite3_errmsg(db)
                << std::endl;
      connected = false;
      return false;
    }

    connected = true;
    initializeTables();
    return true;
  }

  void disconnect() {
    if (connected) {
      sqlite3_close(db);
      connected = false;
    }
  }

  bool isConnected() const { return connected; }

  sqlite3 *getDb() const { return db; }

  void initializeTables() {
    const char *createWastePickupTable =
        "CREATE TABLE IF NOT EXISTS waste_pickups("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "waste_type TEXT NOT NULL,"
        "pickup_location TEXT NOT NULL,"
        "pickup_date_time TEXT NOT NULL,"
        "status TEXT NOT NULL,"
        "user_name TEXT NOT NULL,"
        "creation_timestamp TEXT NOT NULL);";

    char *errMsg = nullptr;
    int rc =
        sqlite3_exec(db, createWastePickupTable, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
      std::cerr << "SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }
  }

  ~Database() { disconnect(); }
};
