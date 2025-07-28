#pragma once
#include <string>
#include <map>
#include <mutex>
#include <atomic>

class CANListener {
public:
    CANListener(const std::string &canIface, const std::map<int, std::string>& idToWarning);
    void run();
    bool getLatestWarning(std::string &warningType);
    std::string getCANBasedTimestamp() const;

    // Getter methods for time-related fields
    int getYear() const { return year_; }
    int getMonth() const { return month_; }
    int getDay() const { return day_; }
    int getHour() const { return hour_; }
    int getMinute() const { return minute_; }
    int getSecond() const { return second_; }

    int getVehicleSpeed() const { return vehicleSpeed_; }
    int getTripMileage() const { return tripMileage_; }
    int getTotalMileage() const { return totalMileage_; }

private:
    std::string canIface_;
    std::map<int, std::string> idToWarning_;
    std::mutex mtx_;
    std::atomic<bool> newWarning_;
    std::string lastWarningType_;

    std::atomic<int> vehicleSpeed_; // ESC_V_VEH
    std::atomic<int> tripMileage_; // Trip_A
    std::atomic<int> totalMileage_; // kilometerstand
    std::atomic<int> hour_, minute_, second_, day_, month_, year_; // Uhrzeit_datum
};