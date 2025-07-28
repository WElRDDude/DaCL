#include "StorageManager.hpp"
#include "FileManager.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

StorageManager::StorageManager(const std::string &bufferDir, int maxMinutes)
    : bufferDir_(bufferDir), maxMinutes_(maxMinutes) {
  // Input validation
  if (bufferDir.empty()) {
    throw std::invalid_argument("Buffer directory path cannot be empty");
  }
  if (maxMinutes <= 0) {
    throw std::invalid_argument("Maximum minutes must be positive");
  }
}

void StorageManager::run() {
  try {
    FileManager fm(bufferDir_,
                   "/tmp"); // Use temporary event dir for cleanup operations

    while (true) {
      try {
        fm.cleanOldSegments(maxMinutes_);
      } catch (const std::exception &e) {
        std::cerr << "Warning: Storage cleanup failed: " << e.what()
                  << std::endl;
        // Continue running even if cleanup fails
      }

      std::this_thread::sleep_for(
          std::chrono::seconds(CLEANUP_INTERVAL_SECONDS));
    }
  } catch (const std::exception &e) {
    std::cerr << "Fatal error in StorageManager: " << e.what() << std::endl;
    throw; // Re-throw to allow calling code to handle
  }
}