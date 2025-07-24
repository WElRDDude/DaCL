#include "TriggerManager.hpp"
#include "utils.hpp"
#include <wiringPi.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>

TriggerManager::TriggerManager(VideoRecorder* vr, FileManager* fm, CSVLogger* cl, OverlayRenderer* overlayRenderer, CANListener* can, int gpioPin, int preMin, int postMin)
    : videoRecorder_(vr), fileManager_(fm), csvLogger_(cl), overlayRenderer_(overlayRenderer), canListener_(can),
      gpioPin_(gpioPin), preMin_(preMin), postMin_(postMin), running_(true) {}

void TriggerManager::run() {
    wiringPiSetup();
    pinMode(gpioPin_, INPUT);
    pullUpDnControl(gpioPin_, PUD_UP);

    std::thread gpioThread(&TriggerManager::handleGPIOTrigger, this);
    std::thread canThread(&TriggerManager::handleCANTrigger, this);
    std::thread consoleThread(&TriggerManager::handleConsoleTrigger, this);

    gpioThread.join();
    canThread.join();
    consoleThread.join();
}

void TriggerManager::handleGPIOTrigger() {
    while (running_) {
        if (digitalRead(gpioPin_) == LOW) {
            std::string triggerType = "GPIO_BUTTON";
            std::string warningType = "Manual Trigger";
            int speed = 55; // Simulated
            std::string timestamp = currentTimestamp();

            auto preFiles = videoRecorder_->getBufferedSegments(preMin_);
            std::string postFile;
            videoRecorder_->startPostTriggerRecording(postMin_, warningType, postFile);

            if (!postFile.empty() && std::filesystem::exists(postFile)) { // Ensure postFile exists
                std::string overlayFile = overlayRenderer_->renderOverlay(speed, warningType, timestamp);
                fileManager_->copyEventSegments(preFiles, warningType, timestamp, overlayFile, "pretrigger");
                fileManager_->copyEventSegments({postFile}, warningType, timestamp, overlayFile, "posttrigger");

                csvLogger_->logEvent(timestamp, triggerType, warningType, speed, preFiles, postFile);
            } else {
                std::cerr << "Error: Post-trigger file does not exist: " << postFile << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void TriggerManager::handleCANTrigger() {
    while (running_) {
        std::string warningType;
        if (canListener_->getLatestWarning(warningType)) {
            std::string triggerType = "CAN";
            int speed = 60; // Simulated
            std::string timestamp = currentTimestamp();

            auto preFiles = videoRecorder_->getBufferedSegments(preMin_);
            std::string postFile;
            videoRecorder_->startPostTriggerRecording(postMin_, warningType, postFile);

            if (!postFile.empty() && std::filesystem::exists(postFile)) { // Ensure postFile exists
                std::string overlayFile = overlayRenderer_->renderOverlay(speed, warningType, timestamp);
                fileManager_->copyEventSegments(preFiles, warningType, timestamp, overlayFile, "pretrigger");
                fileManager_->copyEventSegments({postFile}, warningType, timestamp, overlayFile, "posttrigger");

                csvLogger_->logEvent(timestamp, triggerType, warningType, speed, preFiles, postFile);
            } else {
                std::cerr << "Error: Post-trigger file does not exist: " << postFile << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void TriggerManager::handleConsoleTrigger() {
    while (running_) {
        if (std::cin.peek() == 't') {
            std::cin.get();
            std::string triggerType = "CONSOLE";
            std::string warningType = "Manual Terminal";
            int speed = 50; // Simulated
            std::string timestamp = currentTimestamp();

            auto preFiles = videoRecorder_->getBufferedSegments(preMin_);
            std::string postFile;
            videoRecorder_->startPostTriggerRecording(postMin_, warningType, postFile);

            if (!postFile.empty() && std::filesystem::exists(postFile)) { // Ensure postFile exists
                std::string overlayFile = overlayRenderer_->renderOverlay(speed, warningType, timestamp);
                fileManager_->copyEventSegments(preFiles, warningType, timestamp, overlayFile, "pretrigger");
                fileManager_->copyEventSegments({postFile}, warningType, timestamp, overlayFile, "posttrigger");

                csvLogger_->logEvent(timestamp, triggerType, warningType, speed, preFiles, postFile);
            } else {
                std::cerr << "Error: Post-trigger file does not exist: " << postFile << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}