#include "StorageManager.h"
#include <filesystem>
#include <ctime>
#include <iostream>

StorageManager::StorageManager(const std::string& path) : storagePath(path) {
    std::filesystem::create_directory(storagePath);
}

void StorageManager::saveEventVideo(const std::vector<std::string>& chunks, const std::string& eventName) {
    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::localtime(&now);
    char datetime[32];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M:%S", now_tm);

    std::string eventDir = storagePath + "/" + std::string(datetime) + "_" + eventName;
    std::filesystem::create_directory(eventDir);

    std::string status = eventName.find("_post") != std::string::npos ? "post-error" : "pre-error";
    std::string errorType = eventName.find("manual_trigger") != std::string::npos ? "manual_trigger" : "can_warning";

    for (size_t i = 0; i < chunks.size(); ++i) {
        std::string newName = std::string(datetime) + "_" + errorType + "_" + status + "_" + std::to_string(i) + ".mp4";
        try {
            if (std::filesystem::exists(chunks[i])) {
                std::filesystem::copy(chunks[i], eventDir + "/" + newName);
            } else {
                std::cerr << "File not found: " << chunks[i] << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error copying file: " << e.what() << std::endl;
        }
    }
}
