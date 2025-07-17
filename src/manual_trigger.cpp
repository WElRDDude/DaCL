#include "manual_trigger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

ManualTrigger::ManualTrigger(int gpio_pin) : gpio_pin_(gpio_pin), running_(false) {}

ManualTrigger::~ManualTrigger() { stop(); }

void ManualTrigger::start(TriggerCallback cb) {
    callback_ = cb;
    running_ = true;
    monitor_thread_ = std::thread(&ManualTrigger::monitor_loop, this);
}

void ManualTrigger::stop() {
    running_ = false;
    if (monitor_thread_.joinable()) monitor_thread_.join();
}

void ManualTrigger::monitor_loop() {
    while (running_) {
        // Placeholder: Poll GPIO pin. Replace with actual GPIO logic.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto t = std::time(nullptr);
        if (t % 90 == 0) { // Simulate manual trigger every 90 seconds
            callback_(std::to_string(t));
        }
    }
}