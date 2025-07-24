#include "VideoRecorder.hpp"
#include "utils.hpp"
#include <filesystem>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream> // Added to fix std::cerr error

VideoRecorder::VideoRecorder(const std::string &bufferDir, int segmentSeconds, int bufferMinutes)
    : bufferDir_(bufferDir), segmentSeconds_(segmentSeconds), bufferMinutes_(bufferMinutes),
      postTriggerActive_(false), postTriggerSegmentsLeft_(0) {}

void VideoRecorder::run() {
    while (true) {
        std::string timestamp = currentTimestamp();
        std::string videoFile = bufferDir_ + "/video_" + timestamp + ".mp4";
        std::string cmd = "libcamera-vid --nopreview -t " + std::to_string(segmentSeconds_ * 1000) +
                          " -o " + videoFile + " --codec h264";
        system(cmd.c_str());

        {
            std::lock_guard<std::mutex> lk(mtx_);
            bufferFiles_.push_back(videoFile);
            if ((int)bufferFiles_.size() > bufferMinutes_ * 60 / segmentSeconds_)
                std::filesystem::remove(bufferFiles_.front()), bufferFiles_.erase(bufferFiles_.begin());
            if (postTriggerActive_ && postTriggerSegmentsLeft_ > 0) {
                std::string postFile = bufferDir_ + "/posttrigger_" + timestamp + "_" + eventType_ + ".mp4";
                try {
                    std::filesystem::copy(videoFile, postFile, std::filesystem::copy_options::overwrite_existing);
                    postTriggerFile_ = postFile;
                    postTriggerSegmentsLeft_--;
                    if (postTriggerSegmentsLeft_ == 0)
                        postTriggerActive_ = false;
                } catch (const std::filesystem::filesystem_error &e) {
                    std::cerr << "Error copying post-trigger file: " << e.what() << std::endl;
                }
            }
        }
    }
}

std::vector<std::string> VideoRecorder::getBufferedSegments(int minutesBack) {
    std::lock_guard<std::mutex> lk(mtx_);
    int numSegments = minutesBack * 60 / segmentSeconds_;
    if (numSegments > static_cast<int>(bufferFiles_.size())) // Fixed signed/unsigned comparison
        numSegments = bufferFiles_.size();
    return std::vector<std::string>(bufferFiles_.end() - numSegments, bufferFiles_.end());
}

void VideoRecorder::startPostTriggerRecording(int minutesForward, const std::string &eventType, std::string &postFileOut) {
    std::lock_guard<std::mutex> lk(mtx_);
    postTriggerActive_ = true;
    postTriggerSegmentsLeft_ = minutesForward * 60 / segmentSeconds_;
    eventType_ = eventType;
    postFileOut = postTriggerFile_;
}