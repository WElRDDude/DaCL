#pragma once
#include <string>
#include <map>
#include <mutex>
#include <atomic>

class CANListener {
public:
    CANListener(const std::string &canIface, const std::map<int, std::string>& idToWarning);
    void run();
    bool getLatestWarning(std::string &warningType);
private:
    std::string canIface_;
    std::map<int, std::string> idToWarning_;
    std::mutex mtx_;
    std::atomic<bool> newWarning_;
    std::string lastWarningType_;
};