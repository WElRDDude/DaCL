#pragma once
#include <string>
#include <map>

struct Config {
    int segmentSeconds;
    int bufferMinutes;
    int pretriggerMinutes;
    int posttriggerMinutes;
    std::string bufferDir;
    std::string eventDir;
    std::string canIface;
    std::string warningIds;
    int buttonPin;

    Config(const std::string &filename);
};

std::string currentTimestamp();
std::map<int, std::string> parseCANWarnings(const std::string& warningsString);