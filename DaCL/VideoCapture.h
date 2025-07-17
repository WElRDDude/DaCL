#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include "CircularBuffer.h"
#include <thread>
#include <atomic>

class VideoCapture {
private:
    CircularBuffer& buffer;
    int chunkDuration;
    std::atomic<bool> isRecording;
    std::thread recordingThread;

public:
    VideoCapture(CircularBuffer& buf, int duration);
    void startRecording();
    void stopRecording();
    ~VideoCapture();
};

#endif // VIDEO_CAPTURE_H
