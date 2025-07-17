#include "VideoCapture.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <filesystem>
#include <atomic>
#include <csignal>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

// Helper: find newest files in directory with given extension, sorted by creation time
static std::vector<std::string> get_new_chunks(const std::string& dir, const std::string& ext, std::vector<std::string>& known) {
    std::vector<std::string> new_files;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ext) continue;
        std::string fname = entry.path().string();
        if (std::find(known.begin(), known.end(), fname) == known.end()) {
            new_files.push_back(fname);
        }
    }
    return new_files;
}

VideoCapture::VideoCapture(CircularBuffer& buf, int duration)
    : buffer(buf), chunkDuration(duration), isRecording(false) {}

void VideoCapture::startRecording() {
    if (isRecording) return; // Already running
    isRecording = true;

    // This will store the list of files we've already seen in the buffer dir
    std::string buffer_dir = "./video_chunks";
    if (!fs::exists(buffer_dir)) fs::create_directory(buffer_dir);

    // Remove any old files in buffer dir
    for (auto& entry : fs::directory_iterator(buffer_dir)) {
        if (entry.is_regular_file()) {
            fs::remove(entry.path());
        }
    }

    // Start libcamera-vid ONCE in segment mode
    // It will continuously write new chunk files every chunkDuration seconds
    std::string segment_pattern = buffer_dir + "/chunk_%Y-%m-%d_%H-%M-%S.h264";
    std::string cmd = "libcamera-vid -t 0 --segment " + std::to_string(chunkDuration)
        + " --width 1456 --height 1088 --framerate 15"
        + " -o \"" + segment_pattern + "\" &";
    int ret = std::system(cmd.c_str());
    if (ret != 0) {
        std::cerr << "Failed to start libcamera-vid. Is it installed and working?" << std::endl;
        isRecording = false;
        return;
    }

    recordingThread = std::thread([this, buffer_dir]() {
        std::vector<std::string> known_chunks;
        while (isRecording) {
            // Find new .h264 chunks
            auto new_chunks = get_new_chunks(buffer_dir, ".h264", known_chunks);
            for (const auto& h264_filename : new_chunks) {
                // Convert to .mp4
                std::string mp4_filename = h264_filename.substr(0, h264_filename.size() - 5) + ".mp4";
                std::string convert_cmd = "ffmpeg -y -framerate 15 -i \"" + h264_filename + "\" -c copy \"" + mp4_filename + "\" -loglevel error";
                int convertResult = std::system(convert_cmd.c_str());
                if (convertResult != 0) {
                    std::cerr << "Error executing ffmpeg command" << std::endl;
                    continue;
                }

                // Add to buffer
                buffer.addChunk(mp4_filename);

                // Remove .h264 to save space
                std::string remove_cmd = "rm \"" + h264_filename + "\"";
                int removeResult = std::system(remove_cmd.c_str());
                if (removeResult != 0) {
                    std::cerr << "Error removing .h264 file" << std::endl;
                }

                // Track this chunk as processed
                known_chunks.push_back(h264_filename);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        // Optionally: kill libcamera-vid on exit
        std::system("pkill -f 'libcamera-vid -t 0'");
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