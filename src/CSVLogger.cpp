#include "CSVLogger.hpp"
#include <fstream>
#include <sstream>

CSVLogger::CSVLogger(const std::string &csvFile) : csvFile_(csvFile) {}

void CSVLogger::logEvent(const std::string &timestamp, const std::string &triggerType,
                  const std::string &warningType, int speed,
                  const std::vector<std::string> &preFiles, const std::string &postFile) {
    std::ofstream csv(csvFile_, std::ios::app);
    if (!csv.is_open()) {
        return;
    }

    csv << timestamp << "," << triggerType << "," << warningType << "," << speed << ",";
    for (const auto &f : preFiles)
        csv << f << ";";
    csv << "," << postFile << "\n";
}