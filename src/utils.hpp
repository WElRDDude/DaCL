/**
 * @file utils.hpp
 * @brief Utility functions and configuration management for DaCL system
 */

#pragma once
#include <string>
#include <map>
#include <cstdint>
#include "CANListener.hpp"

/**
 * @struct Config
 * @brief Configuration container for all DaCL system parameters
 * 
 * This structure holds all configurable parameters loaded from the INI file:
 * - Video recording settings (segment duration, buffer size)
 * - Event capture timing (pre/post trigger durations)
 * - Directory paths for buffer and event storage
 * - CAN interface configuration
 * - GPIO pin assignments
 */
struct Config {
    int segmentSeconds;        ///< Duration of each video segment in seconds
    int bufferMinutes;         ///< Total buffer duration in minutes
    int pretriggerMinutes;     ///< Pre-trigger capture duration in minutes
    int posttriggerMinutes;    ///< Post-trigger capture duration in minutes
    std::string bufferDir;     ///< Directory for video segment buffer
    std::string eventDir;      ///< Directory for saved event videos
    std::string canIface;      ///< CAN interface name (e.g., "can0")
    std::string warningIds;    ///< CAN ID to warning type mappings
    int buttonPin;             ///< GPIO pin number for manual trigger button

    /**
     * @brief Constructs Config by loading parameters from INI file
     * @param filename Path to configuration INI file
     * @throws std::runtime_error if file cannot be read or is malformed
     * @throws std::invalid_argument if required parameters are missing
     */
    explicit Config(const std::string &filename);
};

/**
 * @brief Generates current timestamp string using CAN or system time
 * @param canListener Pointer to CAN listener for timestamp data (can be nullptr)
 * @return Formatted timestamp string (YYYYMMDD_HHMMSS)
 * 
 * @note If CAN listener is available and has valid time data, uses CAN timestamp.
 *       Otherwise falls back to system time.
 */
std::string currentTimestamp(const CANListener* canListener = nullptr);

/**
 * @brief Parses CAN warning ID mappings from configuration string
 * @param warningsString Semicolon-separated list of "ID,Name" pairs
 * @return Map of CAN message IDs to warning type strings
 * @throws std::invalid_argument if string format is invalid
 * 
 * Example input: "0x488,WarningMsg_ACM;0x481,WarningMsg_BCM"
 * Returns map: {0x488 -> "WarningMsg_ACM", 0x481 -> "WarningMsg_BCM"}
 */
std::map<int, std::string> parseCANWarnings(const std::string& warningsString);

/**
 * @brief Extracts signal value from CAN message data using bit field specification
 * @param data Pointer to CAN message data bytes
 * @param startBit Starting bit position (0-based)
 * @param length Number of bits to extract
 * @param isLittleEndian True for little-endian byte order, false for big-endian
 * @param factor Scaling factor to apply to extracted value
 * @param offset Offset to add after scaling
 * @return Extracted and processed signal value
 * 
 * @note This function handles complex bit field extraction common in automotive CAN protocols.
 *       Supports both little-endian and big-endian bit ordering as defined in DBC files.
 */
uint32_t extractSignal(const uint8_t* data, 
                      int startBit, 
                      int length, 
                      bool isLittleEndian, 
                      double factor = 1.0, 
                      double offset = 0.0);