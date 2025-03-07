#pragma once

#include "../view/waste_pickup_view.h"
#include <crow.h>

class WastePickupController {
private:
  WastePickupView view;

public:
  WastePickupController(sqlite3 *db);

  // Register endpoints with Crow
  void registerRoutes(crow::App<> &app);

private:
  // Handler methods
  crow::response createPickup(const crow::request &req);
  crow::response getPickupById(int id);
  crow::response getAllPickups();
  crow::response updatePickup(int id, const crow::request &req);
  crow::response deletePickup(int id);
  crow::response getEnvironmentalData();
};
