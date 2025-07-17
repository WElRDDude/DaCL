#ifndef VIDEO_OVERLAY_H
#define VIDEO_OVERLAY_H

#include <opencv2/opencv.hpp>
#include <map>
#include <string>

class VideoOverlay {
public:
    void addOverlay(cv::Mat& frame, const std::map<std::string, std::string>& canData);
};

#endif // VIDEO_OVERLAY_H
