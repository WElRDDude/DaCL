#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "CircularBuffer.h"
#include "VideoCapture.h"
#include "CANBusListener.h"
#include "ManualTrigger.h"
#include "VideoOverlay.h"
#include "StorageManager.h"
#include <termios.h>
#include <unistd.h>

bool running = true;

class LabCarVideoSystem {
private:
    CircularBuffer buffer;
    VideoCapture videoCapture;
    CANBusListener canListener;
    ManualTrigger manualTrigger;
    VideoOverlay videoOverlay;
    StorageManager storageManager;

public:
    LabCarVideoSystem()
        : buffer(20), // 20 chunks of 30 seconds = 10 minutes
          videoCapture(buffer, 30),
          manualTrigger(17), // Example GPIO pin
          storageManager("./events") {}

    void run() {
        videoCapture.startRecording();

        while (running) {
            // Check for manual trigger
            if (manualTrigger.checkTrigger()) {
                handleEvent("manual_trigger");
            }

            // Check for CAN warning
            auto canData = canListener.getCANData();
            if (canListener.checkWarning()) {
                handleEvent("can_warning");
            }

            // Simulate processing delay
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        videoCapture.stopRecording();
    }

    void handleEvent(const std::string& eventName) {
        int chunksFor5Minutes = 10; // Assuming 30-second chunks
        int startIndex = buffer.getCurrentSize() - chunksFor5Minutes;
        if (startIndex < 0) startIndex = 0;

        auto relevantChunks = buffer.getChunks(startIndex, chunksFor5Minutes * 2); // 5 min before and 5 min after
        storageManager.saveEventVideo(relevantChunks, eventName);

        // Wait for the next 5 minutes and save those chunks too
        std::this_thread::sleep_for(std::chrono::seconds(5 * 60)); // Wait for 5 minutes
        auto futureChunks = buffer.getChunks(buffer.getCurrentSize() - chunksFor5Minutes, chunksFor5Minutes);
        storageManager.saveEventVideo(futureChunks, eventName + "_post");
    }
};

int main() {
    std::cout << "Lab Car Video System started" << std::endl;
    std::cout << "Press 'e' in the terminal to stop the program gracefully." << std::endl;

    LabCarVideoSystem system;
    std::thread systemThread([&system]() {
        system.run();
    });

    while (running) {
        // Check for terminal input
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        if (ch == 'e') {
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Shutting down gracefully..." << std::endl;
    // Add any cleanup code here

    if (systemThread.joinable()) {
        systemThread.join();
    }

    return 0;
}
