#pragma once
#include <string>
#include <map>
#include <cstdint>
#include "CANListener.hpp"

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

std::string currentTimestamp(const CANListener* canListener);
std::map<int, std::string> parseCANWarnings(const std::string& warningsString);
uint32_t extractSignal(const uint8_t* data, int startBit, int length, bool isLittleEndian, double factor, double offset);