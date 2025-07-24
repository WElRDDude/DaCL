#pragma once
#include <string>
#include <vector>
class FileManager {
public:
    FileManager(const std::string &bufferDir, const std::string &eventDir);
    void copyEventSegments(const std::vector<std::string> &segments, const std::string &warningType, const std::string &timestamp, const std::string &overlayFile, const std::string &suffix);
    void cleanOldSegments(int maxMinutes);
private:
    std::string bufferDir_, eventDir_;
};