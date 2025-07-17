#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include <string>

class CANBus {
public:
    using EventCallback = std::function<void(const std::string& event_type, const std::string& timestamp)>;

    CANBus(const std::string& can_device);
    ~CANBus();

    void start(EventCallback cb);
    void stop();

private:
    void listen_loop();
    EventCallback callback_;
    std::string can_device_;
    std::atomic<bool> running_;
    std::thread listen_thread_;
};