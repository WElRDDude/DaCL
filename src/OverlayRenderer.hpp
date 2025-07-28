#pragma once
#include <string>
#include "CANListener.hpp"

class OverlayRenderer {
public:
    OverlayRenderer(CANListener* canListener);
    std::string renderOverlay(int speed, const std::string &warningType, const std::string &timestamp);

private:
    CANListener* canListener_; // Pointer to CANListener
};