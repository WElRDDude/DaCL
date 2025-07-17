#include "main_app.hpp"
#include "video_capture.hpp"
#include "can_bus.hpp"
#include "manual_trigger.hpp"
#include "storage.hpp"
#include "circular_buffer.hpp"

#include <iostream>
#include <chrono>
#include <thread> // Add this include

MainApp::MainApp() {}

void MainApp::run() {
    // Configurations (could be read from config file)
    std::string video_device = "/dev/video0";
    std::string output_dir = "./buffer";
    std::string event_dir = "./events";
    int segment_seconds = 60;
    int buffer_minutes = 15;
    int gpio_pin = 17;
    std::string can_device = "can0";

    CircularBuffer buffer(15); // 15 segments
    VideoCapture video(video_device, output_dir, segment_seconds, buffer_minutes);
    Storage storage(event_dir, 100);
    CANBus can(can_device);
    ManualTrigger trigger(gpio_pin);

    // Start modules
    video.start();
    can.start([&](const std::string& event_type, const std::string& timestamp){
        std::cout << "CAN Event Detected: " << event_type << std::endl;
        // Spawn a thread for saving, so main thread continues recording
        std::thread([&, event_type, timestamp](){
            auto files = video.get_recent_segments(300, 300, timestamp);
            storage.save_event(files, event_type, timestamp, "CAN WARNING");
        }).detach(); // Detach so it runs independently
    });
    trigger.start([&](const std::string& timestamp){
        std::cout << "Manual Trigger Detected" << std::endl;
        // Spawn a thread for saving, so main thread continues recording
        std::thread([&, timestamp](){
            auto files = video.get_recent_segments(300, 300, timestamp);
            storage.save_event(files, "MANUAL", timestamp, "MANUAL TRIGGER");
        }).detach();
    });

    std::cout << "System running. Press Ctrl+C to exit." << std::endl;
    while (true) std::this_thread::sleep_for(std::chrono::seconds(10));
}