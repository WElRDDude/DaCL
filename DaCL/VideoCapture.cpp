#include "VideoCapture.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

VideoCapture::VideoCapture(CircularBuffer& buf, int duration)
    : buffer(buf), chunkDuration(duration), isRecording(false) {}

void VideoCapture::startRecording() {
    isRecording = true;
    recordingThread = std::thread([this]() {
        while (isRecording) {
            std::time_t now = std::time(nullptr);
            std::tm* now_tm = std::localtime(&now);
            char datetime[32];
            strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M:%S", now_tm);
            std::string base_name = std::string(datetime);

            std::string h264_filename = base_name + "_normal_pre-error.h264";
            std::string mp4_filename = base_name + "_normal_pre-error.mp4";
            std::string record_cmd = "libcamera-vid -t " + std::to_string(chunkDuration * 1000) + " -o " + h264_filename + " --width 1456 --height 1088 --framerate 15";
            system(record_cmd.c_str());
            std::string convert_cmd = "ffmpeg -y -framerate 15 -i " + h264_filename + " -c copy " + mp4_filename;
            system(convert_cmd.c_str());
            buffer.addChunk(mp4_filename);
            // Optionally, remove the .h264 file after conversion
            std::string remove_cmd = "rm " + h264_filename;
            system(remove_cmd.c_str());
        }
    });
}

void VideoCapture::stopRecording() {
    isRecording = false;
    if (recordingThread.joinable()) {
        recordingThread.join();
    }
}

VideoCapture::~VideoCapture() {
    stopRecording();
}
