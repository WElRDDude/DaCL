#include "utils.hpp"
#include "CANListener.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <iostream>
#include <stdexcept>

Config::Config(const std::string &filename) {
    // Default configuration values
    static constexpr int DEFAULT_SEGMENT_SECONDS = 60;
    static constexpr int DEFAULT_BUFFER_MINUTES = 10;
    static constexpr int DEFAULT_PRETRIGGER_MINUTES = 5;
    static constexpr int DEFAULT_POSTTRIGGER_MINUTES = 5;
    static constexpr int DEFAULT_BUTTON_PIN = 0;
    static constexpr const char* DEFAULT_BUFFER_DIR = "/tmp/dacl_buffer";
    static constexpr const char* DEFAULT_EVENT_DIR = "/tmp/dacl_events";
    static constexpr const char* DEFAULT_CAN_IFACE = "can0";
    
    // Initialize with defaults
    segmentSeconds = DEFAULT_SEGMENT_SECONDS;
    bufferMinutes = DEFAULT_BUFFER_MINUTES;
    pretriggerMinutes = DEFAULT_PRETRIGGER_MINUTES;
    posttriggerMinutes = DEFAULT_POSTTRIGGER_MINUTES;
    bufferDir = DEFAULT_BUFFER_DIR;
    eventDir = DEFAULT_EVENT_DIR;
    canIface = DEFAULT_CAN_IFACE;
    warningIds = "";
    buttonPin = DEFAULT_BUTTON_PIN;

    // Input validation
    if (filename.empty()) {
        throw std::invalid_argument("Configuration filename cannot be empty");
    }

    std::ifstream f(filename);
    if (!f.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + filename);
    }
    
    std::string line;
    std::map<std::string, std::string> kv;
    
    while (std::getline(f, line)) {
        const auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        const auto key = line.substr(0, pos);
        const auto value = line.substr(pos + 1);
        kv[key] = value;
    }
    
    // Parse configuration values with error handling
    try {
        if (kv.count("segment_seconds")) {
            segmentSeconds = std::stoi(kv["segment_seconds"]);
            if (segmentSeconds <= 0) {
                throw std::invalid_argument("segment_seconds must be positive");
            }
        }
        
        if (kv.count("buffer_minutes")) {
            bufferMinutes = std::stoi(kv["buffer_minutes"]);
            if (bufferMinutes <= 0) {
                throw std::invalid_argument("buffer_minutes must be positive");
            }
        }
        
        if (kv.count("pretrigger_minutes")) {
            pretriggerMinutes = std::stoi(kv["pretrigger_minutes"]);
            if (pretriggerMinutes < 0) {
                throw std::invalid_argument("pretrigger_minutes cannot be negative");
            }
        }
        
        if (kv.count("posttrigger_minutes")) {
            posttriggerMinutes = std::stoi(kv["posttrigger_minutes"]);
            if (posttriggerMinutes < 0) {
                throw std::invalid_argument("posttrigger_minutes cannot be negative");
            }
        }
        
        if (kv.count("buffer_dir")) {
            bufferDir = kv["buffer_dir"];
            if (bufferDir.empty()) {
                throw std::invalid_argument("buffer_dir cannot be empty");
            }
        }
        
        if (kv.count("event_dir")) {
            eventDir = kv["event_dir"];
            if (eventDir.empty()) {
                throw std::invalid_argument("event_dir cannot be empty");
            }
        }
        
        if (kv.count("can_iface")) {
            canIface = kv["can_iface"];
            if (canIface.empty()) {
                throw std::invalid_argument("can_iface cannot be empty");
            }
        }
        
        if (kv.count("warning_ids")) {
            warningIds = kv["warning_ids"];
        }
        
        if (kv.count("button_pin")) {
            buttonPin = std::stoi(kv["button_pin"]);
            if (buttonPin < 0) {
                throw std::invalid_argument("button_pin cannot be negative");
            }
        }
        
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Configuration parsing error: " + std::string(e.what()));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Configuration value out of range: " + std::string(e.what()));
    }
}

std::string currentTimestamp(const CANListener* canListener) {
    static constexpr size_t TIMESTAMP_BUFFER_SIZE = 32;
    char buf[TIMESTAMP_BUFFER_SIZE];
    
    if (canListener != nullptr) {
        // Use CAN-based timestamp if available
        std::snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d",
                     canListener->getYear(), canListener->getMonth(), canListener->getDay(),
                     canListener->getHour(), canListener->getMinute(), canListener->getSecond());
    } else {
        // Fall back to system time
        const auto now = std::time(nullptr);
        const auto* tm = std::localtime(&now);
        if (tm == nullptr) {
            throw std::runtime_error("Failed to get local time");
        }
        
        std::snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d",
                     tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                     tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    
    return std::string(buf);
}

std::map<int, std::string> parseCANWarnings(const std::string& warningsString) {
    std::map<int, std::string> result;
    
    if (warningsString.empty()) {
        return result; // Return empty map for empty input
    }
    
    std::stringstream ss(warningsString);
    std::string item;
    
    while (std::getline(ss, item, ';')) {
        const auto pos = item.find(',');
        if (pos != std::string::npos) {
            try {
                const std::string idStr = item.substr(0, pos);
                const std::string label = item.substr(pos + 1);
                
                if (idStr.empty() || label.empty()) {
                    std::cerr << "Warning: Skipping malformed CAN warning entry: " << item << std::endl;
                    continue;
                }
                
                // Parse ID with support for hex notation (0x prefix)
                const int id = std::stoi(idStr, nullptr, 0);
                result[id] = label;
                
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse CAN warning entry '" << item 
                         << "': " << e.what() << std::endl;
                // Continue processing other entries
            }
        } else {
            std::cerr << "Warning: Skipping malformed CAN warning entry (missing comma): " 
                     << item << std::endl;
        }
    }
    
    return result;
}

uint32_t extractSignal(const uint8_t* data, 
                      int startBit, 
                      int length, 
                      bool isLittleEndian, 
                      double factor, 
                      double offset) {
    // Input validation
    if (data == nullptr) {
        throw std::invalid_argument("Data pointer cannot be null");
    }
    if (startBit < 0) {
        throw std::invalid_argument("Start bit cannot be negative");
    }
    if (length <= 0 || length > 32) {
        throw std::invalid_argument("Length must be between 1 and 32 bits");
    }
    
    static constexpr int BITS_PER_BYTE = 8;
    static constexpr int MAX_CAN_FRAME_SIZE = 8; // Standard CAN frame size
    
    // Check if signal fits within reasonable CAN frame bounds
    const int endBit = startBit + length - 1;
    if (endBit >= MAX_CAN_FRAME_SIZE * BITS_PER_BYTE) {
        throw std::invalid_argument("Signal extends beyond standard CAN frame size");
    }
    
    uint64_t rawValue = 0;
    
    // Calculate byte and bit positions
    const int startByte = startBit / BITS_PER_BYTE;
    const int startBitInByte = startBit % BITS_PER_BYTE;
    
    // Extract bytes based on endianness
    for (int i = 0; i < (length + startBitInByte + BITS_PER_BYTE - 1) / BITS_PER_BYTE; ++i) {
        if (isLittleEndian) {
            // Little-endian: LSB first
            rawValue |= static_cast<uint64_t>(data[startByte + i]) << (i * BITS_PER_BYTE);
        } else {
            // Big-endian: MSB first  
            rawValue = (rawValue << BITS_PER_BYTE) | data[startByte + i];
        }
    }

    // Shift to align signal to LSB and apply bit mask
    rawValue >>= startBitInByte;
    rawValue &= (1ULL << length) - 1;

    // Apply scaling factor and offset
    const double scaledValue = static_cast<double>(rawValue) * factor + offset;
    
    // Check for reasonable output range
    if (scaledValue < 0 || scaledValue > static_cast<double>(UINT32_MAX)) {
        std::cerr << "Warning: Scaled signal value " << scaledValue 
                 << " is outside uint32_t range" << std::endl;
    }
    
    return static_cast<uint32_t>(scaledValue);
}