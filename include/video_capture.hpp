#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>

class VideoCapture {
public:
    VideoCapture(const std::string& device, const std::string& output_dir, int segment_seconds, int buffer_minutes);
    ~VideoCapture();

    void start();
    void stop();
    std::vector<std::string> get_recent_segments(int seconds_before, int seconds_after, const std::string& event_time);

    // Overlay info to be updated by other modules
    void set_overlay_info(const std::string& info);

private:
    void capture_loop();
    std::string device_;
    std::string output_dir_;
    int segment_seconds_;
    int buffer_minutes_;
    std::atomic<bool> running_;
    std::thread capture_thread_;
    std::string overlay_info_;
};