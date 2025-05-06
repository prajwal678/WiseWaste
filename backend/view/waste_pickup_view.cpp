#include "waste_pickup_view.h"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    struct WasteImpact {
        double waste_kg;
        double co2_reduction_kg;
    };
    
    const std::map<std::string, WasteImpact> WASTE_IMPACT_FACTORS = {
        {"Plastic",    {2.5,  5.0}},
        {"Electronic", {5.0, 20.0}},
        {"Hazardous",  {1.0, 10.0}},
        {"Organic",    {3.0,  3.0}}
    };
}

WastePickupView::WastePickupView(sqlite3 *db) : db(db) {}

bool WastePickupView::createPickupRequest(const std::string &wasteType,
                                          const std::string &pickupLocation,
                                          const std::string &pickupDateTime,
                                          const std::string &userName) {
  if (wasteType.empty() || pickupLocation.empty() || pickupDateTime.empty() ||
      userName.empty()) {
    return false;
  }

  try {
    WastePickup pickup(-1, WastePickup::stringToWasteType(wasteType), 
                      pickupLocation, std::chrono::system_clock::now(),
                      PickupStatus::PENDING, userName);
    return WastePickup::create(db, pickup);
  } catch (const std::exception& e) {
    std::cerr << "Error creating pickup request: " << e.what() << std::endl;
    return false;
  }
}

std::unique_ptr<WastePickup> WastePickupView::getPickupById(int id) {
  return WastePickup::getById(db, id);
}

std::vector<std::unique_ptr<WastePickup>> WastePickupView::getAllPickups() {
  return WastePickup::getAll(db);
}

std::vector<std::unique_ptr<WastePickup>>
WastePickupView::getPickupsByStatus(const std::string &status) {
  return WastePickup::getByStatus(db, WastePickup::stringToStatus(status));
}

std::vector<std::unique_ptr<WastePickup>>
WastePickupView::getPickupsByUser(const std::string &userName) {
  return WastePickup::getByUser(db, userName);
}

bool WastePickupView::updatePickupRequest(int id, const std::string &wasteType,
                                          const std::string &pickupLocation,
                                          const std::string &pickupDateTime,
                                          const std::string &status,
                                          const std::string &userName) {
  if (wasteType.empty() || pickupLocation.empty() || pickupDateTime.empty() ||
      status.empty() || userName.empty() || id <= 0) {
    return false;
  }

  auto existingPickup = WastePickup::getById(db, id);
  if (!existingPickup) {
    return false;
  }

  existingPickup->setWasteType(WastePickup::stringToWasteType(wasteType));
  existingPickup->setPickupLocation(pickupLocation);
  
  std::tm tm = {};
  std::stringstream ss(pickupDateTime);
  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
  existingPickup->setPickupDateTime(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
  
  existingPickup->setStatus(WastePickup::stringToStatus(status));
  existingPickup->setUserName(userName);

  return WastePickup::update(db, *existingPickup);
}

bool WastePickupView::cancelPickupRequest(int id) {
  if (id <= 0) {
    return false;
  }

  return WastePickup::remove(db, id);
}

void WastePickupView::checkAndUpdateCompletedStatus() {
  WastePickup::updateCompletedStatus(db);
}

std::string WastePickupView::getEnvironmentalImpactData() {
  json impact;
  
  try {
    auto completedPickups = WastePickup::getByStatus(db, PickupStatus::COMPLETED);

    double totalRecycledWaste = 0.0;
    double co2Reduction = 0.0;
    std::map<std::string, int> wasteTypeCount;

    for (const auto &pickup : completedPickups) {
      const std::string& wasteType = WastePickup::wasteTypeToString(pickup->getWasteType());
      wasteTypeCount[wasteType]++;

      auto it = WASTE_IMPACT_FACTORS.find(wasteType);
      if (it != WASTE_IMPACT_FACTORS.end()) {
        totalRecycledWaste += it->second.waste_kg;
        co2Reduction += it->second.co2_reduction_kg;
      }
    }

    impact["totalRecycledWaste"] = totalRecycledWaste;
    impact["co2Reduction"] = co2Reduction;
    impact["wasteTypeDistribution"] = wasteTypeCount;
    impact["totalCompletedPickups"] = completedPickups.size();

    return impact.dump();
  } catch (const std::exception& e) {
    std::cerr << "Error calculating environmental impact: " << e.what() << std::endl;
    return json({{"error", "Failed to calculate environmental impact"}}).dump();
  }
}
