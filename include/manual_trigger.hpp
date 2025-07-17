#pragma once
#include <functional>
#include <atomic>
#include <thread>

class ManualTrigger {
public:
    using TriggerCallback = std::function<void(const std::string& timestamp)>;

    ManualTrigger(int gpio_pin);
    ~ManualTrigger();
    void start(TriggerCallback cb);
    void stop();

private:
    void monitor_loop();
    int gpio_pin_;
    std::atomic<bool> running_;
    std::thread monitor_thread_;
    TriggerCallback callback_;
};