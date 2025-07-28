/**
 * @file StorageManager.hpp
 * @brief Automatic storage management for video buffer maintenance
 */

#pragma once
#include <string>

/**
 * @class StorageManager
 * @brief Manages storage cleanup to maintain video buffer within size limits
 * 
 * This class runs as a background service to:
 * - Monitor buffer directory disk usage
 * - Remove old video segments when buffer exceeds time limits
 * - Ensure continuous operation without storage exhaustion
 * 
 * @note This class should be run in a separate thread to avoid blocking main operations
 */
class StorageManager final {
public:
    /**
     * @brief Constructs a StorageManager for the specified buffer directory
     * @param bufferDir Directory path containing video segments to manage
     * @param maxMinutes Maximum buffer duration in minutes before cleanup
     * @throws std::invalid_argument if bufferDir is empty or maxMinutes <= 0
     */
    explicit StorageManager(const std::string &bufferDir, int maxMinutes);
    
    /**
     * @brief Main loop for periodic storage cleanup
     * @note This method runs indefinitely with periodic cleanup cycles.
     *       Should be executed in a separate thread.
     */
    void run();

private:
    const std::string bufferDir_;  ///< Directory to monitor and clean
    const int maxMinutes_;         ///< Maximum buffer duration before cleanup
    
    static constexpr int CLEANUP_INTERVAL_SECONDS = 60;  ///< Cleanup check interval
};