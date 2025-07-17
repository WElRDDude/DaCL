#include "CANBusListener.h"
#include <ctime>

CANBusListener::CANBusListener() : rng(std::random_device()()), speedDist(0, 120), warningDist(0, 99), hasWarning(false) {}

std::map<std::string, std::string> CANBusListener::getCANData() {
    std::map<std::string, std::string> data;
    data["speed"] = std::to_string(speedDist(rng));
    int warningChance = warningDist(rng);
    if (warningChance < 5) {
        hasWarning = true;
        data["warning"] = "Engine Fault";
    } else if (warningChance < 10) {
        hasWarning = true;
        data["warning"] = "Brake Warning";
    } else {
        hasWarning = false;
        data["warning"] = "None";
    }
    return data;
}

bool CANBusListener::checkWarning() {
    return hasWarning;
}
