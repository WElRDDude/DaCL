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
            int recordResult = system(record_cmd.c_str());
            if (recordResult != 0) {
                std::cerr << "Error executing libcamera-vid command" << std::endl;
                continue;
            }

            std::string convert_cmd = "ffmpeg -y -framerate 15 -i " + h264_filename + " -c copy " + mp4_filename;
            int convertResult = system(convert_cmd.c_str());
            if (convertResult != 0) {
                std::cerr << "Error executing ffmpeg command" << std::endl;
                continue;
            }

            buffer.addChunk(mp4_filename);

            std::string remove_cmd = "rm " + h264_filename;
            int removeResult = system(remove_cmd.c_str());
            if (removeResult != 0) {
                std::cerr << "Error removing .h264 file" << std::endl;
            }
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
