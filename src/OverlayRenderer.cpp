#include "OverlayRenderer.hpp"
#include <opencv2/opencv.hpp>
#include <filesystem>

std::string OverlayRenderer::renderOverlay(int speed, const std::string &warningType, const std::string &timestamp) {
    cv::Mat img(80, 640, CV_8UC3, cv::Scalar(0,0,0));
    cv::putText(img, "Speed: " + std::to_string(speed) + " km/h", {10,30}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0,255,0}, 2);
    cv::putText(img, "Warning: " + warningType, {10,60}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0,0,255}, 2);
    cv::putText(img, "Time: " + timestamp, {350,30}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {255,255,255}, 1);

    std::string overlayFile = "/tmp/dacl_overlay_" + timestamp + ".png";
    cv::imwrite(overlayFile, img);
    return overlayFile;
}