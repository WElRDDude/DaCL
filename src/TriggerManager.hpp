#pragma once
#include "VideoRecorder.hpp"
#include "FileManager.hpp"
#include "CSVLogger.hpp"
#include "OverlayRenderer.hpp"
#include "CANListener.hpp"
#include <atomic>

class TriggerManager {
public:
    TriggerManager(VideoRecorder*, FileManager*, CSVLogger*, OverlayRenderer*, CANListener*, int gpioPin, int preMin, int postMin);
    void run();
    
private:
    void handleGPIOTrigger();
    void handleCANTrigger();
    void handleConsoleTrigger();
    VideoRecorder *videoRecorder_;
    FileManager *fileManager_;
    CSVLogger *csvLogger_;
    OverlayRenderer *overlayRenderer_;
    CANListener *canListener_;
    int gpioPin_;
    int preMin_, postMin_;
    std::atomic<bool> running_;
};