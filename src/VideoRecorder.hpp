#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

class VideoRecorder {
public:
    VideoRecorder(const std::string &bufferDir, int segmentSeconds, int bufferMinutes);
    void run();
    std::vector<std::string> getBufferedSegments(int minutesBack);
    void startPostTriggerRecording(int minutesForward, const std::string &eventType, std::string &postFileOut);
private:
    std::string bufferDir_;
    int segmentSeconds_;
    int bufferMinutes_;
    std::vector<std::string> bufferFiles_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool postTriggerActive_;
    int postTriggerSegmentsLeft_;
    std::string eventType_;
    std::string postTriggerFile_;
};