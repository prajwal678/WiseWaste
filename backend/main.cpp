#include "controller/waste_pickup_controller.h"
#include "model/database.h"
#include "model/waste_pickup.h"
#include <chrono>
#include <crow.h>
#include <iostream>
#include <thread>

int main() {
  // Initialize database
  Database db;
  if (!db.connect()) {
    std::cerr << "Failed to connect to database. Exiting..." << std::endl;
    return 1;
  }

  std::cout << "Database connected successfully." << std::endl;

  // Initialize controller
  WastePickupController controller(db.getDb());

  // Initialize Crow application
  crow::SimpleApp app;

  // Enable CORS
  CROW_ROUTE(app, "/<path>")
    .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "OPTIONS"_method)
    ([](const crow::request& req) {
      crow::response res;
      res.add_header("Access-Control-Allow-Origin", "*");
      res.add_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS");
      res.add_header("Access-Control-Allow-Headers", "X-Requested-With, Content-Type, Accept");
      return res;
    });

  // Register routes
  controller.registerRoutes(app);

  // Start background thread to periodically check for completed pickups
  std::thread bgThread([db = std::ref(db)]() {
    while (true) {
      // Check every 5 minutes
      std::this_thread::sleep_for(std::chrono::minutes(5));
      WastePickup::updateCompletedStatus(db.get().getDb());
    }
  });
  bgThread.detach();

  // Start the server
  app.port(8000).multithreaded().run();

  return 0;
}
