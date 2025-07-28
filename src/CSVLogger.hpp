/**
 * @file CSVLogger.hpp
 * @brief CSV-based event logging for audit trail and analysis
 */

#pragma once
#include <string>
#include <vector>

/**
 * @class CSVLogger
 * @brief Manages CSV-formatted logging of all trigger events and metadata
 * 
 * This class provides structured logging functionality:
 * - Records all trigger events with comprehensive metadata
 * - Maintains CSV format for easy analysis and processing
 * - Thread-safe logging operations for concurrent access
 * - Automatic header generation and file management
 * 
 * CSV format includes: timestamp, trigger type, warning type, speed, 
 * pre-trigger files, post-trigger files, and additional metadata.
 */
class CSVLogger final {
public:
    /**
     * @brief Constructs a CSVLogger for the specified CSV file
     * @param csvFile Path to CSV log file (will be created if it doesn't exist)
     * @throws std::invalid_argument if csvFile path is empty
     * @throws std::runtime_error if file cannot be opened for writing
     */
    explicit CSVLogger(const std::string &csvFile);
    
    /**
     * @brief Logs a trigger event with comprehensive metadata
     * @param timestamp Event timestamp (YYYYMMDD_HHMMSS format)
     * @param triggerType Source of trigger ("CAN", "GPIO", "Console")
     * @param warningType Type of warning or event
     * @param speed Vehicle speed at time of event
     * @param preFiles List of pre-trigger video files saved
     * @param postFile Post-trigger video file saved
     * @throws std::runtime_error if logging operation fails
     * 
     * @note This method is thread-safe and can be called concurrently
     */
    void logEvent(const std::string &timestamp, 
                  const std::string &triggerType,
                  const std::string &warningType, 
                  int speed,
                  const std::vector<std::string> &preFiles, 
                  const std::string &postFile);

private:
    const std::string csvFile_;  ///< Path to CSV log file
    
    /**
     * @brief Ensures CSV file exists with proper header
     * @throws std::runtime_error if file operations fail
     */
    void ensureHeaderExists();
};