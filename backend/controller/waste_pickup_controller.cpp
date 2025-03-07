#include "waste_pickup_controller.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

WastePickupController::WastePickupController(sqlite3 *db) : view(db) {
  // Check for completed pickups periodically
  view.checkAndUpdateCompletedStatus();
}

void WastePickupController::registerRoutes(crow::App<> &app) {
  // GET /api/wastepickups
  CROW_ROUTE(app, "/api/wastepickups")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request &req) { return this->getAllPickups(); });

  // GET /api/wastepickups/<id>
  CROW_ROUTE(app, "/api/wastepickups/<int>")
      .methods(crow::HTTPMethod::GET)(
          [this](int id) { return this->getPickupById(id); });

  // POST /api/wastepickups
  CROW_ROUTE(app, "/api/wastepickups")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request &req) { return this->createPickup(req); });

  // PUT /api/wastepickups/<id>
  CROW_ROUTE(app, "/api/wastepickups/<int>")
      .methods(crow::HTTPMethod::PUT)([this](const crow::request &req, int id) {
        return this->updatePickup(id, req);
      });

  // DELETE /api/wastepickups/<id>
  CROW_ROUTE(app, "/api/wastepickups/<int>")
      .methods(crow::HTTPMethod::DELETE)(
          [this](int id) { return this->deletePickup(id); });

  // GET /api/environmental-impact
  CROW_ROUTE(app, "/api/environmental-impact")
      .methods(crow::HTTPMethod::GET)(
          [this]() { return this->getEnvironmentalData(); });
}

crow::response WastePickupController::createPickup(const crow::request &req) {
  auto bodyStr = req.body;

  try {
    json body = json::parse(bodyStr);

    if (!body.contains("wasteType") || !body.contains("pickupLocation") ||
        !body.contains("pickupDateTime") || !body.contains("userName")) {

      return crow::response(400, "Missing required fields");
    }

    std::string wasteType = body["wasteType"];
    std::string location = body["pickupLocation"];
    std::string dateTime = body["pickupDateTime"];
    std::string userName = body["userName"];

    bool success =
        view.createPickupRequest(wasteType, location, dateTime, userName);
