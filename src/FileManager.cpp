#include "FileManager.hpp"
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

FileManager::FileManager(const std::string &bufferDir, const std::string &eventDir)
    : bufferDir_(bufferDir), eventDir_(eventDir) {}

void FileManager::copyEventSegments(const std::vector<std::string> &segments, const std::string &warningType, const std::string &timestamp, const std::string &overlayFile, const std::string &suffix) {
    std::filesystem::create_directories(eventDir_); // Ensure the event directory exists

    for (size_t i = 0; i < segments.size(); ++i) {
        std::string dest = eventDir_ + "/" + timestamp + "_" + warningType + "_" + suffix + "_" + std::to_string(i) + ".mp4";
        std::filesystem::copy(segments[i], dest, std::filesystem::copy_options::overwrite_existing);

        // Overlay using ffmpeg, e.g.:
        std::string tempDest = dest + "_temp.mp4";
        std::string cmd = "ffmpeg -y -i " + dest + " -vf \"movie=" + overlayFile + " [overlay]; [in][overlay] overlay=0:0 [out]\" -codec:a copy " + tempDest + " && mv " + tempDest + " " + dest;
        int ret = system(cmd.c_str());
        if (ret != 0) {
            std::cerr << "Error: ffmpeg command failed for " << dest << std::endl;
        }
    }
}

void FileManager::cleanOldSegments(int maxMinutes) {
    using namespace std::chrono;
    auto now = system_clock::now();
    for (auto &p : std::filesystem::directory_iterator(bufferDir_)) {
        auto ftime = std::filesystem::last_write_time(p);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        auto age = std::chrono::duration_cast<std::chrono::minutes>(now - sctp);
        if (age.count() > maxMinutes) {
            std::filesystem::remove(p);
        }
    }
}