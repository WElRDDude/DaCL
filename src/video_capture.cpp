#include "video_capture.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstdlib>

VideoCapture::VideoCapture(const std::string& device, const std::string& output_dir, int segment_seconds, int buffer_minutes)
    : device_(device), output_dir_(output_dir), segment_seconds_(segment_seconds), buffer_minutes_(buffer_minutes), running_(false) {}

VideoCapture::~VideoCapture() { stop(); }

void VideoCapture::start() {
    running_ = true;
    capture_thread_ = std::thread(&VideoCapture::capture_loop, this);
}

void VideoCapture::stop() {
    running_ = false;
    if (capture_thread_.joinable()) capture_thread_.join();
}

void VideoCapture::capture_loop() {
    using namespace std::chrono;
    while (running_) {
        auto t = system_clock::now();
        auto tt = system_clock::to_time_t(t);

        std::ostringstream filename;
        filename << output_dir_ << "/segment_" << tt << ".mp4";

        // Example: use libcamera-vid, piping overlay info if available
        std::ostringstream cmd;
        cmd << "libcamera-vid -t " << (segment_seconds_ * 1000)
            << " --output " << filename.str();
        if (!overlay_info_.empty())
            cmd << " --annotate '" << overlay_info_ << "'";
        std::system(cmd.str().c_str());

        std::this_thread::sleep_for(seconds(segment_seconds_));
    }
}

std::vector<std::string> VideoCapture::get_recent_segments(int seconds_before, int seconds_after, const std::string& event_time) {
    // Implementation: list files in output_dir_ within event window
    // (Placeholder: actual implementation would filter files by timestamp)
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(output_dir_)) {
        files.push_back(entry.path().string());
    }
    return files;
}

void VideoCapture::set_overlay_info(const std::string& info) {
    overlay_info_ = info;
}