#include "VideoOverlay.h"
#include <ctime>
#include <iostream>

void VideoOverlay::addOverlay(cv::Mat& frame, const std::map<std::string, std::string>& canData) {
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    timestamp.erase(timestamp.find_last_not_of("\n") + 1);

    cv::putText(frame, "Time: " + timestamp, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

    try {
        cv::putText(frame, "Speed: " + canData.at("speed") + " km/h", cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        cv::putText(frame, "Warning: " + canData.at("warning"), cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    } catch (const std::out_of_range& e) {
        std::cerr << "Missing CAN data key: " << e.what() << std::endl;
    }
}
