#pragma once
#include <string>
class PreviewManager {
public:
    PreviewManager(const std::string &bufferDir);
    void run();
private:
    std::string bufferDir_;
};