#pragma once
#include <string>
class StorageManager {
public:
    StorageManager(const std::string &bufferDir, int maxMinutes);
    void run();
private:
    std::string bufferDir_;
    int maxMinutes_;
};