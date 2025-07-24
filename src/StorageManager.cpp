#include "StorageManager.hpp"
#include "FileManager.hpp"
#include <thread>
#include <chrono>

StorageManager::StorageManager(const std::string &bufferDir, int maxMinutes)
    : bufferDir_(bufferDir), maxMinutes_(maxMinutes) {}

void StorageManager::run() {
    FileManager fm(bufferDir_, "");
    while (true) {
        fm.cleanOldSegments(maxMinutes_);
        std::this_thread::sleep_for(std::chrono::minutes(2));
    }
}