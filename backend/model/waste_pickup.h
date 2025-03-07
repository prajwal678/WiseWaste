#pragma once

#include "database.h"
#include <chrono>
#include <memory>
#include <string>
#include <vector>

enum class WasteType {
    PLASTIC,
    ELECTRONIC,
    ORGANIC,
    HAZARDOUS
};

enum class PickupStatus {
    PENDING,
    SCHEDULED,
    COMPLETED
};

class WastePickup {
public:
    // Constructors
    WastePickup();
    explicit WastePickup(int id, WasteType type, const std::string& location,
                        const std::chrono::system_clock::time_point& dateTime,
                        PickupStatus status, const std::string& userName);

    // Getters
    int getId() const;
    WasteType getWasteType() const;
    std::string getPickupLocation() const;
    std::chrono::system_clock::time_point getPickupDateTime() const;
    PickupStatus getStatus() const;
    std::string getUserName() const;

    // Setters
    void setWasteType(WasteType type);
    void setPickupLocation(const std::string& location);
    void setPickupDateTime(const std::chrono::system_clock::time_point& dateTime);
    void setStatus(PickupStatus status);
    void setUserName(const std::string& name);

    // Database operations
    static bool create(sqlite3* db, const WastePickup& pickup);
    static std::unique_ptr<WastePickup> getById(sqlite3* db, int id);
    static std::vector<std::unique_ptr<WastePickup>> getByStatus(sqlite3* db, PickupStatus status);
    static std::vector<std::unique_ptr<WastePickup>> getByUser(sqlite3* db, const std::string& userName);
    static std::vector<std::unique_ptr<WastePickup>> getAll(sqlite3* db);
    static bool update(sqlite3* db, const WastePickup& pickup);
    static bool remove(sqlite3* db, int id);
    static void updateCompletedStatus(sqlite3* db);

    // JSON conversion
    std::string toJson() const;
    static std::unique_ptr<WastePickup> fromJson(const std::string& jsonStr);

    // Helper methods
    static std::string wasteTypeToString(WasteType type);
    static WasteType stringToWasteType(const std::string& str);
    static std::string statusToString(PickupStatus status);
    static PickupStatus stringToStatus(const std::string& str);

private:
    int id;
    WasteType wasteType;
    std::string pickupLocation;
    std::chrono::system_clock::time_point pickupDateTime;
    PickupStatus status;
    std::string userName;
};
