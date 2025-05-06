#pragma once

#include "../model/waste_pickup.h"
#include <map>
#include <memory>
#include <sqlite3.h>
#include <string>
#include <vector>

class WastePickupView {
private:
  sqlite3 *db;

public:
  WastePickupView(sqlite3 *db);

  bool createPickupRequest(const std::string &wasteType,
                           const std::string &pickupLocation,
                           const std::string &pickupDateTime,
                           const std::string &userName);

  std::unique_ptr<WastePickup> getPickupById(int id);
  std::vector<std::unique_ptr<WastePickup>> getAllPickups();
  std::vector<std::unique_ptr<WastePickup>>
  getPickupsByStatus(const std::string &status);
  std::vector<std::unique_ptr<WastePickup>>
  getPickupsByUser(const std::string &userName);

  bool updatePickupRequest(int id, const std::string &wasteType,
                           const std::string &pickupLocation,
                           const std::string &pickupDateTime,
                           const std::string &status,
                           const std::string &userName);

  bool cancelPickupRequest(int id);

  void checkAndUpdateCompletedStatus();
  std::string getEnvironmentalImpactData();
};
