#pragma once

#include "model/waste_pickup.h"
#include <crow.h>
#include <sqlite3.h>

class WastePickupController {
public:
    explicit WastePickupController(sqlite3* db);
    void registerRoutes(crow::SimpleApp& app);

private:
    sqlite3* db;

    // Route handlers
    crow::response getAllPickups();
    crow::response getPickupById(int id);
    crow::response createPickup(const crow::request& req);
    crow::response updatePickup(const crow::request& req, int id);
    crow::response deletePickup(int id);
};
