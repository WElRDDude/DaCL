#ifndef CAN_BUS_LISTENER_H
#define CAN_BUS_LISTENER_H

#include <map>
#include <string>
#include <random>

class CANBusListener {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> speedDist;
    std::uniform_int_distribution<int> warningDist;
    bool hasWarning;

public:
    CANBusListener();
    std::map<std::string, std::string> getCANData();
    bool checkWarning();
};

#endif // CAN_BUS_LISTENER_H
