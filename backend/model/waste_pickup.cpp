#include "waste_pickup.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

WastePickup::WastePickup() : id(-1), status("Pending") {
  // Get current timestamp
  auto now = std::chrono::system_clock::now();
  auto nowTime = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
  creationTimestamp = ss.str();
}

WastePickup::WastePickup(const std::string &wasteType,
                         const std::string &pickupLocation,
                         const std::string &pickupDateTime,
                         const std::string &status, const std::string &userName)
    : id(-1), wasteType(wasteType), pickupLocation(pickupLocation),
      pickupDateTime(pickupDateTime), status(status), userName(userName) {

  // Get current timestamp
  auto now = std::chrono::system_clock::now();
  auto nowTime = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
  creationTimestamp = ss.str();
}

bool WastePickup::create(sqlite3 *db, const WastePickup &pickup) {
  const char *insertSQL =
      "INSERT INTO waste_pickups (waste_type, pickup_location, "
      "pickup_date_time, status, user_name, creation_timestamp) "
      "VALUES (?, ?, ?, ?, ?, ?)";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return false;
  }

  sqlite3_bind_text(stmt, 1, pickup.wasteType.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, pickup.pickupLocation.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, pickup.pickupDateTime.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, pickup.status.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 5, pickup.userName.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 6, pickup.creationTimestamp.c_str(), -1,
                    SQLITE_STATIC);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return rc == SQLITE_DONE;
}

std::unique_ptr<WastePickup> WastePickup::readById(sqlite3 *db, int id) {
  const char *selectSQL = "SELECT * FROM waste_pickups WHERE id = ?";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return nullptr;
  }

  sqlite3_bind_int(stmt, 1, id);

  rc = sqlite3_step(stmt);

  if (rc == SQLITE_ROW) {
    auto pickup = std::make_unique<WastePickup>();

    pickup->id = sqlite3_column_int(stmt, 0);
    pickup->wasteType =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    pickup->pickupLocation =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    pickup->pickupDateTime =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    pickup->status =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    pickup->userName =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    pickup->creationTimestamp =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

    sqlite3_finalize(stmt);
    return pickup;
  }

  sqlite3_finalize(stmt);
  return nullptr;
}

std::vector<std::unique_ptr<WastePickup>>
WastePickup::readByStatus(sqlite3 *db, const std::string &status) {
  std::vector<std::unique_ptr<WastePickup>> results;
  const char *selectSQL = "SELECT * FROM waste_pickups WHERE status = ?";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return results;
  }

  sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    auto pickup = std::make_unique<WastePickup>();

    pickup->id = sqlite3_column_int(stmt, 0);
    pickup->wasteType =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    pickup->pickupLocation =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    pickup->pickupDateTime =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    pickup->status =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    pickup->userName =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    pickup->creationTimestamp =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

    results.push_back(std::move(pickup));
  }

  sqlite3_finalize(stmt);
  return results;
}

std::vector<std::unique_ptr<WastePickup>>
WastePickup::readByUser(sqlite3 *db, const std::string &userName) {
  std::vector<std::unique_ptr<WastePickup>> results;
  const char *selectSQL = "SELECT * FROM waste_pickups WHERE user_name = ?";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return results;
  }

  sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    auto pickup = std::make_unique<WastePickup>();

    pickup->id = sqlite3_column_int(stmt, 0);
    pickup->wasteType =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    pickup->pickupLocation =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    pickup->pickupDateTime =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    pickup->status =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    pickup->userName =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    pickup->creationTimestamp =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

    results.push_back(std::move(pickup));
  }

  sqlite3_finalize(stmt);
  return results;
}

std::vector<std::unique_ptr<WastePickup>> WastePickup::readAll(sqlite3 *db) {
  std::vector<std::unique_ptr<WastePickup>> results;
  const char *selectSQL = "SELECT * FROM waste_pickups";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return results;
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    auto pickup = std::make_unique<WastePickup>();

    pickup->id = sqlite3_column_int(stmt, 0);
    pickup->wasteType =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    pickup->pickupLocation =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    pickup->pickupDateTime =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    pickup->status =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    pickup->userName =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    pickup->creationTimestamp =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

    results.push_back(std::move(pickup));
  }

  sqlite3_finalize(stmt);
  return results;
}

bool WastePickup::update(sqlite3 *db, const WastePickup &pickup) {
  const char *updateSQL = "UPDATE waste_pickups SET "
                          "waste_type = ?, "
                          "pickup_location = ?, "
                          "pickup_date_time = ?, "
                          "status = ?, "
                          "user_name = ? "
                          "WHERE id = ?";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return false;
  }

  sqlite3_bind_text(stmt, 1, pickup.wasteType.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, pickup.pickupLocation.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, pickup.pickupDateTime.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, pickup.status.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 5, pickup.userName.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 6, pickup.id);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return rc == SQLITE_DONE;
}

bool WastePickup::deletePickup(sqlite3 *db, int id) {
  const char *deleteSQL = "DELETE FROM waste_pickups WHERE id = ?";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, deleteSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return false;
  }

  sqlite3_bind_int(stmt, 1, id);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return rc == SQLITE_DONE;
}

std::string WastePickup::toJson() const {
  json j;
  j["id"] = id;
  j["wasteType"] = wasteType;
  j["pickupLocation"] = pickupLocation;
  j["pickupDateTime"] = pickupDateTime;
  j["status"] = status;
  j["userName"] = userName;
  j["creationTimestamp"] = creationTimestamp;

  return j.dump();
}

std::unique_ptr<WastePickup> WastePickup::fromJson(const std::string &jsonStr) {
  try {
    json j = json::parse(jsonStr);

    auto pickup = std::make_unique<WastePickup>();

    pickup->wasteType = j["wasteType"];
    pickup->pickupLocation = j["pickupLocation"];
    pickup->pickupDateTime = j["pickupDateTime"];
    pickup->status = j.value("status", "Pending");
    pickup->userName = j["userName"];

    if (j.contains("id")) {
      pickup->id = j["id"];
    }

    return pickup;
  } catch (const std::exception &e) {
    std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    return nullptr;
  }
}

void WastePickup::updateCompletedStatus(sqlite3 *db) {
  // Get current timestamp
  auto now = std::chrono::system_clock::now();
  auto nowTime = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
  std::string currentDateTime = ss.str();

  // Update pickups where scheduled time has passed and status is not Completed
  const char *updateSQL =
      "UPDATE waste_pickups SET status = 'Completed' "
      "WHERE status != 'Completed' AND pickup_date_time < ?";

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL preparation error: " << sqlite3_errmsg(db) << std::endl;
    return;
  }

  sqlite3_bind_text(stmt, 1, currentDateTime.c_str(), -1, SQLITE_STATIC);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}
