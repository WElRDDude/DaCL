/**
 * @file CANListener.hpp
 * @brief CAN bus interface for automotive data and warning message processing
 */

#pragma once
#include <string>
#include <map>
#include <mutex>
#include <atomic>

/**
 * @class CANListener
 * @brief Handles CAN bus communication for receiving vehicle data and warning messages
 * 
 * This class provides a thread-safe interface to the CAN bus system, capable of:
 * - Listening for warning messages from various vehicle ECUs
 * - Extracting vehicle speed, mileage, and timestamp data
 * - Parsing bit fields from CAN message payloads
 * - Thread-safe access to latest received data
 * 
 * @note Thread Safety: All getter methods are thread-safe using atomic variables.
 *       The run() method should be executed in a separate thread.
 */
class CANListener final {
public:
    /**
     * @brief Constructs a CANListener with specified interface and warning mappings
     * @param canIface CAN interface name (e.g., "can0")
     * @param idToWarning Map of CAN message IDs to warning type strings
     * @throws std::invalid_argument if canIface is empty
     */
    explicit CANListener(const std::string &canIface, const std::map<int, std::string>& idToWarning);
    
    /**
     * @brief Main loop for CAN message processing
     * @note This method runs indefinitely and should be called from a worker thread
     */
    void run();
    
    /**
     * @brief Retrieves the latest warning message if available
     * @param[out] warningType The type of warning received
     * @return true if a new warning was available, false otherwise
     * @note Thread-safe: Can be called from any thread
     */
    bool getLatestWarning(std::string &warningType);
    
    /**
     * @brief Generates a timestamp string based on CAN-received time data
     * @return Formatted timestamp string (YYYYMMDD_HHMMSS)
     * @note Thread-safe: Uses atomic time variables
     */
    std::string getCANBasedTimestamp() const;

    // Time getter methods - all thread-safe using atomic variables
    /** @brief Get current year from CAN timestamp data */
    int getYear() const { return year_; }
    /** @brief Get current month from CAN timestamp data */
    int getMonth() const { return month_; }
    /** @brief Get current day from CAN timestamp data */
    int getDay() const { return day_; }
    /** @brief Get current hour from CAN timestamp data */
    int getHour() const { return hour_; }
    /** @brief Get current minute from CAN timestamp data */
    int getMinute() const { return minute_; }
    /** @brief Get current second from CAN timestamp data */
    int getSecond() const { return second_; }

    // Vehicle data getter methods - all thread-safe using atomic variables
    /** @brief Get current vehicle speed in appropriate units from ESC_V_VEH */
    int getVehicleSpeed() const { return vehicleSpeed_; }
    /** @brief Get trip mileage from Trip_A */
    int getTripMileage() const { return tripMileage_; }
    /** @brief Get total odometer reading from kilometerstand */
    int getTotalMileage() const { return totalMileage_; }

private:
    std::string canIface_;                    ///< CAN interface name
    std::map<int, std::string> idToWarning_;  ///< Mapping of CAN IDs to warning strings
    std::mutex mtx_;                          ///< Mutex for warning data protection
    std::atomic<bool> newWarning_;            ///< Flag indicating new warning available
    std::string lastWarningType_;             ///< Most recent warning type (protected by mtx_)

    // Vehicle data - atomic for thread-safe access
    std::atomic<int> vehicleSpeed_;   ///< Vehicle speed from ESC_V_VEH (0x1A1)
    std::atomic<int> tripMileage_;    ///< Trip mileage from Trip_A (0x3F4)
    std::atomic<int> totalMileage_;   ///< Total mileage from kilometerstand (0x19D)
    
    // Time data - atomic for thread-safe access
    std::atomic<int> hour_, minute_, second_, day_, month_, year_;  ///< Time/date from Uhrzeit_datum (0x2F8)
};