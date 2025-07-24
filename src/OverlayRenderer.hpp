#pragma once
#include <string>

class OverlayRenderer {
public:
    std::string renderOverlay(int speed, const std::string &warningType, const std::string &timestamp);
};