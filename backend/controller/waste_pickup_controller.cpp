#include "waste_pickup_controller.h"
#include <crow.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

WastePickupController::WastePickupController(sqlite3 *db) : db(db) {}

void WastePickupController::registerRoutes(crow::SimpleApp &app) {
  // GET all pickups
  CROW_ROUTE(app, "/api/wastepickups")
      .methods("GET"_method)(
          [this](const crow::request &) { return this->getAllPickups(); });

  // GET pickup by ID
  CROW_ROUTE(app, "/api/wastepickups/<int>")
      .methods("GET"_method)(
          [this](const crow::request &, int id) { return this->getPickupById(id); });

  // POST new pickup
  CROW_ROUTE(app, "/api/wastepickups")
      .methods("POST"_method)(
          [this](const crow::request &req) { return this->createPickup(req); });

  // PUT update pickup
  CROW_ROUTE(app, "/api/wastepickups/<int>")
      .methods("PUT"_method)([this](const crow::request &req, int id) {
        return this->updatePickup(req, id);
      });

  // DELETE pickup
  CROW_ROUTE(app, "/api/wastepickups/<int>")
      .methods("DELETE"_method)(
          [this](const crow::request &, int id) { return this->deletePickup(id); });
}

crow::response WastePickupController::getAllPickups() {
  try {
    auto pickups = WastePickup::getAll(db);
    json response;
    for (const auto &pickup : pickups) {
      response.push_back(json::parse(pickup->toJson()));
    }
    return crow::response(response.dump());
  } catch (const std::exception &e) {
    return crow::response(500, e.what());
  }
}

crow::response WastePickupController::getPickupById(int id) {
  try {
    auto pickup = WastePickup::getById(db, id);
    if (!pickup) {
      return crow::response(404, "Pickup not found");
    }
    return crow::response(pickup->toJson());
  } catch (const std::exception &e) {
    return crow::response(500, e.what());
  }
}

crow::response WastePickupController::createPickup(const crow::request &req) {
  try {
    auto jsonData = json::parse(req.body);
    
    // Validate required fields
    if (!jsonData.contains("wasteType") || !jsonData.contains("pickupLocation") ||
        !jsonData.contains("pickupDateTime") || !jsonData.contains("userName")) {
      return crow::response(400, "Missing required fields");
    }

    // Create pickup object
    auto pickup = WastePickup::fromJson(req.body);
    if (!pickup) {
      return crow::response(400, "Invalid pickup data");
    }

    // Save to database
    if (!WastePickup::create(db, *pickup)) {
      return crow::response(500, "Failed to create pickup");
    }

    return crow::response(201, pickup->toJson());
  } catch (const std::exception &e) {
    return crow::response(500, e.what());
  }
}

crow::response WastePickupController::updatePickup(const crow::request &req, int id) {
  try {
    auto pickup = WastePickup::getById(db, id);
    if (!pickup) {
      return crow::response(404, "Pickup not found");
    }

    auto jsonData = json::parse(req.body);
    if (jsonData.contains("wasteType")) {
      pickup->setWasteType(WastePickup::stringToWasteType(jsonData["wasteType"]));
    }
    if (jsonData.contains("pickupLocation")) {
      pickup->setPickupLocation(jsonData["pickupLocation"]);
    }
    if (jsonData.contains("pickupDateTime")) {
      auto timeStr = jsonData["pickupDateTime"].get<std::string>();
      std::tm tm = {};
      std::stringstream ss(timeStr);
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      pickup->setPickupDateTime(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
    }
    if (jsonData.contains("status")) {
      pickup->setStatus(WastePickup::stringToStatus(jsonData["status"]));
    }
    if (jsonData.contains("userName")) {
      pickup->setUserName(jsonData["userName"]);
    }

    if (!WastePickup::update(db, *pickup)) {
      return crow::response(500, "Failed to update pickup");
    }

    return crow::response(pickup->toJson());
  } catch (const std::exception &e) {
    return crow::response(500, e.what());
  }
}

crow::response WastePickupController::deletePickup(int id) {
  try {
    if (!WastePickup::remove(db, id)) {
      return crow::response(404, "Pickup not found");
    }
    return crow::response(204);
  } catch (const std::exception &e) {
    return crow::response(500, e.what());
  }
}
