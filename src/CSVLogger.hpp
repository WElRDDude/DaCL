#pragma once
#include <string>
#include <vector>
class CSVLogger {
public:
    CSVLogger(const std::string &csvFile);
    void logEvent(const std::string &timestamp, const std::string &triggerType,
                  const std::string &warningType, int speed,
                  const std::vector<std::string> &preFiles, const std::string &postFile);
private:
    std::string csvFile_;
};