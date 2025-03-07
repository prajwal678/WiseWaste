#include "waste_pickup_view.h"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

WastePickupView::WastePickupView(sqlite3 *db) : db(db) {}

bool WastePickupView::createPickupRequest(const std::string &wasteType,
                                          const std::string &pickupLocation,
                                          const std::string &pickupDateTime,
                                          const std::string &userName) {
  // Validate input
  if (wasteType.empty() || pickupLocation.empty() || pickupDateTime.empty() ||
      userName.empty()) {
    return false;
  }

  WastePickup pickup(wasteType, pickupLocation, pickupDateTime, "Pending",
                     userName);
  return WastePickup::create(db, pickup);
}

std::unique_ptr<WastePickup> WastePickupView::getPickupById(int id) {
  return WastePickup::readById(db, id);
}

std::vector<std::unique_ptr<WastePickup>> WastePickupView::getAllPickups() {
  return WastePickup::readAll(db);
}

std::vector<std::unique_ptr<WastePickup>>
WastePickupView::getPickupsByStatus(const std::string &status) {
  return WastePickup::readByStatus(db, status);
}

std::vector<std::unique_ptr<WastePickup>>
WastePickupView::getPickupsByUser(const std::string &userName) {
  return WastePickup::readByUser(db, userName);
}

bool WastePickupView::updatePickupRequest(int id, const std::string &wasteType,
                                          const std::string &pickupLocation,
                                          const std::string &pickupDateTime,
                                          const std::string &status,
                                          const std::string &userName) {
  // Validate input
  if (wasteType.empty() || pickupLocation.empty() || pickupDateTime.empty() ||
      status.empty() || userName.empty() || id <= 0) {
    return false;
  }

  // Get the existing pickup
  auto existingPickup = WastePickup::readById(db, id);
  if (!existingPickup) {
    return false;
  }

  // Update fields
  existingPickup->setWasteType(wasteType);
  existingPickup->setPickupLocation(pickupLocation);
  existingPickup->setPickupDateTime(pickupDateTime);
  existingPickup->setStatus(status);
  existingPickup->setUserName(userName);

  return WastePickup::update(db, *existingPickup);
}

bool WastePickupView::cancelPickupRequest(int id) {
  if (id <= 0) {
    return false;
  }

  return WastePickup::deletePickup(db, id);
}

void WastePickupView::checkAndUpdateCompletedStatus() {
  WastePickup::updateCompletedStatus(db);
}

std::string WastePickupView::getEnvironmentalImpactData() {
  // Calculate environmental impact based on waste types
  json impact;

  // Get all completed pickups
  auto completedPickups = WastePickup::readByStatus(db, "Completed");

  // Calculate metrics
  double totalRecycledWaste = 0.0;
  double co2Reduction = 0.0;
  std::map<std::string, int> wasteTypeCount;

  for (const auto &pickup : completedPickups) {
    wasteTypeCount[pickup->getWasteType()]++;

    // For this example, we'll use arbitrary values for environmental impact
    if (pickup->getWasteType() == "Plastic") {
      totalRecycledWaste += 2.5; // Assuming 2.5 kg per plastic waste pickup
      co2Reduction +=
          5.0; // Assuming 5 kg CO2 reduction per plastic waste pickup
    } else if (pickup->getWasteType() == "Electronic") {
      totalRecycledWaste += 5.0; // Assuming 5 kg per electronic waste pickup
      co2Reduction +=
          20.0; // Assuming 20 kg CO2 reduction per electronic waste pickup
    } else if (pickup->getWasteType() == "Hazardous") {
      totalRecycledWaste += 1.0; // Assuming 1 kg per hazardous waste pickup
      co2Reduction +=
          10.0; // Assuming 10 kg CO2 reduction per hazardous waste pickup
    } else if (pickup->getWasteType() == "Organic") {
      totalRecycledWaste += 3.0; // Assuming 3 kg per organic waste pickup
      co2Reduction +=
          3.0; // Assuming 3 kg CO2 reduction per organic waste pickup
    }
  }

  impact["totalRecycledWaste"] = totalRecycledWaste;
  impact["co2Reduction"] = co2Reduction;
  impact["wasteTypeDistribution"] = wasteTypeCount;
  impact["totalCompletedPickups"] = completedPickups.size();

  return impact.dump();
}
