#pragma once
#include <string>
#include <vector>

class Storage {
public:
    Storage(const std::string& event_dir, size_t max_events);
    void save_event(const std::vector<std::string>& files, const std::string& event_type, const std::string& timestamp, const std::string& overlay_info);
    void cleanup();

private:
    std::string event_dir_;
    size_t max_events_;
};