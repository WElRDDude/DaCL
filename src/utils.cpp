#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>

Config::Config(const std::string &filename) {
    segmentSeconds = 60;
    bufferMinutes = 10;
    pretriggerMinutes = 5;
    posttriggerMinutes = 5;
    bufferDir = "/tmp/dacl_buffer";
    eventDir = "/tmp/dacl_events";
    canIface = "can0";
    warningIds = "";
    buttonPin = 0;

    std::ifstream f(filename);
    std::string line;
    std::map<std::string, std::string> kv;
    while (std::getline(f, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        auto k = line.substr(0, pos);
        auto v = line.substr(pos + 1);
        kv[k] = v;
    }
    if (kv.count("segment_seconds")) segmentSeconds = std::stoi(kv["segment_seconds"]);
    if (kv.count("buffer_minutes")) bufferMinutes = std::stoi(kv["buffer_minutes"]);
    if (kv.count("pretrigger_minutes")) pretriggerMinutes = std::stoi(kv["pretrigger_minutes"]);
    if (kv.count("posttrigger_minutes")) posttriggerMinutes = std::stoi(kv["posttrigger_minutes"]);
    if (kv.count("buffer_dir")) bufferDir = kv["buffer_dir"];
    if (kv.count("event_dir")) eventDir = kv["event_dir"];
    if (kv.count("can_iface")) canIface = kv["can_iface"];
    if (kv.count("warning_ids")) warningIds = kv["warning_ids"];
    if (kv.count("button_pin")) buttonPin = std::stoi(kv["button_pin"]);
}

std::string currentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm *ptm = std::localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", ptm);
    return std::string(buf);
}

std::map<int, std::string> parseCANWarnings(const std::string& warningsString) {
    std::map<int, std::string> result;
    std::stringstream ss(warningsString);
    std::string item;
    while (std::getline(ss, item, ';')) {
        auto pos = item.find(',');
        if (pos != std::string::npos) {
            int id = std::stoi(item.substr(0, pos), nullptr, 0);
            std::string label = item.substr(pos+1);
            result[id] = label;
        }
    }
    return result;
}