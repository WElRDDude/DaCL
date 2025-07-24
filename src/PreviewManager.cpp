#include "PreviewManager.hpp"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

PreviewManager::PreviewManager(const std::string &bufferDir)
    : bufferDir_(bufferDir) {}

void PreviewManager::run() {
    cv::namedWindow("Preview");
    while (true) {
        std::vector<std::string> files;
        for (auto &p : std::filesystem::directory_iterator(bufferDir_)) {
            files.push_back(p.path().string());
        }
        if (!files.empty()) {
            std::sort(files.begin(), files.end());
            std::string file = files.back();
            cv::VideoCapture cap(file);
            cv::Mat frame;
            if (cap.isOpened() && cap.read(frame)) {
                cv::imshow("Preview", frame);
                cv::waitKey(60);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}