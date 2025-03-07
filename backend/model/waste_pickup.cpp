#include "waste_pickup.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <sqlite3.h>

using json = nlohmann::json;

WastePickup::WastePickup() : id(-1), wasteType(WasteType::PLASTIC), 
    pickupDateTime(std::chrono::system_clock::now()), status(PickupStatus::PENDING) {}

WastePickup::WastePickup(int id, WasteType type, const std::string& location,
                        const std::chrono::system_clock::time_point& dateTime,
                        PickupStatus status, const std::string& userName)
    : id(id), wasteType(type), pickupLocation(location), 
      pickupDateTime(dateTime), status(status), userName(userName) {}

// Getters
int WastePickup::getId() const { return id; }
WasteType WastePickup::getWasteType() const { return wasteType; }
std::string WastePickup::getPickupLocation() const { return pickupLocation; }
std::chrono::system_clock::time_point WastePickup::getPickupDateTime() const { return pickupDateTime; }
PickupStatus WastePickup::getStatus() const { return status; }
std::string WastePickup::getUserName() const { return userName; }

// Setters
void WastePickup::setWasteType(WasteType type) { wasteType = type; }
void WastePickup::setPickupLocation(const std::string& location) { pickupLocation = location; }
void WastePickup::setPickupDateTime(const std::chrono::system_clock::time_point& dateTime) { pickupDateTime = dateTime; }
void WastePickup::setStatus(PickupStatus s) { status = s; }
void WastePickup::setUserName(const std::string& name) { userName = name; }

// Database operations
bool WastePickup::create(sqlite3* db, const WastePickup& pickup) {
    const char* sql = "INSERT INTO waste_pickups (waste_type, pickup_location, pickup_datetime, status, user_name) "
                     "VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, wasteTypeToString(pickup.wasteType).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pickup.pickupLocation.c_str(), -1, SQLITE_STATIC);
    
    auto timeStr = std::chrono::system_clock::to_time_t(pickup.pickupDateTime);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&timeStr), "%Y-%m-%d %H:%M:%S");
    sqlite3_bind_text(stmt, 3, ss.str().c_str(), -1, SQLITE_STATIC);
    
    sqlite3_bind_text(stmt, 4, statusToString(pickup.status).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, pickup.userName.c_str(), -1, SQLITE_STATIC);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

std::unique_ptr<WastePickup> WastePickup::getById(sqlite3* db, int id) {
    const char* sql = "SELECT * FROM waste_pickups WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto pickup = std::make_unique<WastePickup>();
    pickup->id = sqlite3_column_int(stmt, 0);
    pickup->wasteType = stringToWasteType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    pickup->pickupLocation = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    
    std::string dateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    std::tm tm = {};
    std::stringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    pickup->pickupDateTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    pickup->status = stringToStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
    pickup->userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

    sqlite3_finalize(stmt);
    return pickup;
}

std::vector<std::unique_ptr<WastePickup>> WastePickup::getByStatus(sqlite3* db, PickupStatus status) {
    const char* sql = "SELECT * FROM waste_pickups WHERE status = ?";
    sqlite3_stmt* stmt;
    std::vector<std::unique_ptr<WastePickup>> pickups;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return pickups;
    }

    sqlite3_bind_text(stmt, 1, statusToString(status).c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto pickup = std::make_unique<WastePickup>();
        pickup->id = sqlite3_column_int(stmt, 0);
        pickup->wasteType = stringToWasteType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        pickup->pickupLocation = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        std::string dateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::tm tm = {};
        std::stringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        pickup->pickupDateTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        pickup->status = stringToStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        pickup->userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        
        pickups.push_back(std::move(pickup));
    }

    sqlite3_finalize(stmt);
    return pickups;
}

std::vector<std::unique_ptr<WastePickup>> WastePickup::getByUser(sqlite3* db, const std::string& userName) {
    const char* sql = "SELECT * FROM waste_pickups WHERE user_name = ?";
    sqlite3_stmt* stmt;
    std::vector<std::unique_ptr<WastePickup>> pickups;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return pickups;
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto pickup = std::make_unique<WastePickup>();
        pickup->id = sqlite3_column_int(stmt, 0);
        pickup->wasteType = stringToWasteType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        pickup->pickupLocation = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        std::string dateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::tm tm = {};
        std::stringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        pickup->pickupDateTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        pickup->status = stringToStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        pickup->userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        
        pickups.push_back(std::move(pickup));
    }

    sqlite3_finalize(stmt);
    return pickups;
}

std::vector<std::unique_ptr<WastePickup>> WastePickup::getAll(sqlite3* db) {
    const char* sql = "SELECT * FROM waste_pickups";
    sqlite3_stmt* stmt;
    std::vector<std::unique_ptr<WastePickup>> pickups;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return pickups;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto pickup = std::make_unique<WastePickup>();
        pickup->id = sqlite3_column_int(stmt, 0);
        pickup->wasteType = stringToWasteType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        pickup->pickupLocation = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        std::string dateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::tm tm = {};
        std::stringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        pickup->pickupDateTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        pickup->status = stringToStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        pickup->userName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        
        pickups.push_back(std::move(pickup));
    }

    sqlite3_finalize(stmt);
    return pickups;
}

bool WastePickup::update(sqlite3* db, const WastePickup& pickup) {
    const char* sql = "UPDATE waste_pickups SET waste_type = ?, pickup_location = ?, "
                     "pickup_datetime = ?, status = ?, user_name = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, wasteTypeToString(pickup.wasteType).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pickup.pickupLocation.c_str(), -1, SQLITE_STATIC);
    
    auto timeStr = std::chrono::system_clock::to_time_t(pickup.pickupDateTime);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&timeStr), "%Y-%m-%d %H:%M:%S");
    sqlite3_bind_text(stmt, 3, ss.str().c_str(), -1, SQLITE_STATIC);
    
    sqlite3_bind_text(stmt, 4, statusToString(pickup.status).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, pickup.userName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, pickup.id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool WastePickup::remove(sqlite3* db, int id) {
    const char* sql = "DELETE FROM waste_pickups WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

void WastePickup::updateCompletedStatus(sqlite3* db) {
    const char* sql = "UPDATE waste_pickups SET status = ? WHERE pickup_datetime < datetime('now') AND status != ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }

    sqlite3_bind_text(stmt, 1, statusToString(PickupStatus::COMPLETED).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, statusToString(PickupStatus::COMPLETED).c_str(), -1, SQLITE_STATIC);
    
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Helper methods
std::string WastePickup::wasteTypeToString(WasteType type) {
    switch (type) {
        case WasteType::PLASTIC: return "PLASTIC";
        case WasteType::ELECTRONIC: return "ELECTRONIC";
        case WasteType::ORGANIC: return "ORGANIC";
        case WasteType::HAZARDOUS: return "HAZARDOUS";
        default: return "UNKNOWN";
    }
}

WasteType WastePickup::stringToWasteType(const std::string& str) {
    if (str == "PLASTIC") return WasteType::PLASTIC;
    if (str == "ELECTRONIC") return WasteType::ELECTRONIC;
    if (str == "ORGANIC") return WasteType::ORGANIC;
    if (str == "HAZARDOUS") return WasteType::HAZARDOUS;
    return WasteType::PLASTIC;
}

std::string WastePickup::statusToString(PickupStatus status) {
    switch (status) {
        case PickupStatus::PENDING: return "PENDING";
        case PickupStatus::SCHEDULED: return "SCHEDULED";
        case PickupStatus::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}

PickupStatus WastePickup::stringToStatus(const std::string& str) {
    if (str == "PENDING") return PickupStatus::PENDING;
    if (str == "SCHEDULED") return PickupStatus::SCHEDULED;
    if (str == "COMPLETED") return PickupStatus::COMPLETED;
    return PickupStatus::PENDING;
}

// JSON conversion
std::string WastePickup::toJson() const {
    std::stringstream ss;
    ss << "{"
       << "\"id\":" << id << ","
       << "\"wasteType\":\"" << wasteTypeToString(wasteType) << "\","
       << "\"pickupLocation\":\"" << pickupLocation << "\","
       << "\"pickupDateTime\":\"" << std::chrono::system_clock::to_time_t(pickupDateTime) << "\","
       << "\"status\":\"" << statusToString(status) << "\","
       << "\"userName\":\"" << userName << "\""
       << "}";
    return ss.str();
}

std::unique_ptr<WastePickup> WastePickup::fromJson(const std::string& jsonStr) {
    // Simple JSON parsing (you might want to use a proper JSON library)
    // This is a basic implementation
    auto pickup = std::make_unique<WastePickup>();
    
    // Extract values using string manipulation
    // This is a simplified version - you should use a proper JSON parser
    size_t pos = jsonStr.find("\"id\":");
    if (pos != std::string::npos) {
        pickup->id = std::stoi(jsonStr.substr(pos + 5));
    }
    
    pos = jsonStr.find("\"wasteType\":\"");
    if (pos != std::string::npos) {
        size_t end = jsonStr.find("\"", pos + 12);
        if (end != std::string::npos) {
            pickup->wasteType = stringToWasteType(jsonStr.substr(pos + 12, end - pos - 12));
        }
    }
    
    pos = jsonStr.find("\"pickupLocation\":\"");
    if (pos != std::string::npos) {
        size_t end = jsonStr.find("\"", pos + 17);
        if (end != std::string::npos) {
            pickup->pickupLocation = jsonStr.substr(pos + 17, end - pos - 17);
        }
    }
    
    pos = jsonStr.find("\"status\":\"");
    if (pos != std::string::npos) {
        size_t end = jsonStr.find("\"", pos + 10);
        if (end != std::string::npos) {
            pickup->status = stringToStatus(jsonStr.substr(pos + 10, end - pos - 10));
        }
    }
    
    pos = jsonStr.find("\"userName\":\"");
    if (pos != std::string::npos) {
        size_t end = jsonStr.find("\"", pos + 11);
        if (end != std::string::npos) {
            pickup->userName = jsonStr.substr(pos + 11, end - pos - 11);
        }
    }
    
    return pickup;
}
