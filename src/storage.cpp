#include "storage.hpp"
#include "overlay.hpp"
#include <filesystem>
#include <sstream>
#include <iostream>

Storage::Storage(const std::string& event_dir, size_t max_events)
    : event_dir_(event_dir), max_events_(max_events) {
    if (!std::filesystem::exists(event_dir_))
        std::filesystem::create_directories(event_dir_);
}

void Storage::save_event(const std::vector<std::string>& files, const std::string& event_type, const std::string& timestamp, const std::string& overlay_info) {
        std::ostringstream dirname;
        dirname << event_dir_ << "/" << event_type << "_" << timestamp;
        std::filesystem::create_directories(dirname.str());
        for (const auto& file : files) {
            std::string output = dirname.str() + "/" + std::filesystem::path(file).filename().string();
            Overlay::overlay_text(file, overlay_info, output);
        }
}

void Storage::cleanup() {
    // Remove old events if over max_events_
    // Placeholder for brevity
}