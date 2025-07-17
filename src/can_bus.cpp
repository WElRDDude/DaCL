#include "can_bus.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

CANBus::CANBus(const std::string& can_device) : can_device_(can_device), running_(false) {}

CANBus::~CANBus() { stop(); }

void CANBus::start(EventCallback cb) {
    callback_ = cb;
    running_ = true;
    listen_thread_ = std::thread(&CANBus::listen_loop, this);
}

void CANBus::stop() {
    running_ = false;
    if (listen_thread_.joinable()) listen_thread_.join();
}

void CANBus::listen_loop() {
    while (running_) {
        // Placeholder: Actual CAN bus reading logic here.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Simulate an event every 60 seconds
        auto t = std::time(nullptr);
        if (t % 60 == 0) {
            callback_("CAN_WARNING", std::to_string(t));
        }
    }
}