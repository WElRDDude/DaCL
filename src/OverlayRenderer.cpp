#include "OverlayRenderer.hpp"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <stdexcept>

OverlayRenderer::OverlayRenderer(CANListener *canListener)
    : canListener_(canListener) {
  if (canListener == nullptr) {
    throw std::invalid_argument("CANListener pointer cannot be null");
  }
}

std::string OverlayRenderer::renderOverlay(int speed,
                                           const std::string &warningType,
                                           const std::string &timestamp) {
  // Input validation
  if (warningType.empty()) {
    throw std::invalid_argument("Warning type cannot be empty");
  }
  if (timestamp.empty()) {
    throw std::invalid_argument("Timestamp cannot be empty");
  }

  // Get vehicle data from CAN listener
  const int tripMileage = canListener_->getTripMileage();
  const int totalMileage = canListener_->getTotalMileage();

  // Create overlay image with defined constants
  cv::Mat img(OVERLAY_HEIGHT, OVERLAY_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));

  // Define text positioning and styling constants
  constexpr int TEXT_START_X = 10;
  constexpr int LINE_HEIGHT = 30;
  constexpr double FONT_SIZE = 0.7;

  // Colors (BGR format)
  const cv::Scalar COLOR_SPEED(0, 255, 0);       // Green for speed
  const cv::Scalar COLOR_TRIP(255, 255, 0);      // Cyan for trip
  const cv::Scalar COLOR_TOTAL(255, 255, 255);   // White for total
  const cv::Scalar COLOR_WARNING(0, 0, 255);     // Red for warning
  const cv::Scalar COLOR_TIMESTAMP(255, 0, 255); // Magenta for timestamp

  // Render text lines with consistent positioning
  cv::putText(img, "Speed: " + std::to_string(speed) + " km/h",
              cv::Point(TEXT_START_X, LINE_HEIGHT), cv::FONT_HERSHEY_SIMPLEX,
              FONT_SIZE, COLOR_SPEED, TEXT_THICKNESS);

  cv::putText(img, "Trip: " + std::to_string(tripMileage) + " km",
              cv::Point(TEXT_START_X, LINE_HEIGHT * 2),
              cv::FONT_HERSHEY_SIMPLEX, FONT_SIZE, COLOR_TRIP, TEXT_THICKNESS);

  cv::putText(img, "Total: " + std::to_string(totalMileage) + " km",
              cv::Point(TEXT_START_X, LINE_HEIGHT * 3),
              cv::FONT_HERSHEY_SIMPLEX, FONT_SIZE, COLOR_TOTAL, 1);

  cv::putText(
      img, "Warning: " + warningType, cv::Point(TEXT_START_X, LINE_HEIGHT * 4),
      cv::FONT_HERSHEY_SIMPLEX, FONT_SIZE, COLOR_WARNING, TEXT_THICKNESS);

  cv::putText(img, "Time: " + timestamp,
              cv::Point(OVERLAY_WIDTH / 2, LINE_HEIGHT),
              cv::FONT_HERSHEY_SIMPLEX, FONT_SIZE, COLOR_TIMESTAMP, 1);

  // Generate overlay file path with error checking
  const std::string overlayFile = "/tmp/dacl_overlay_" + timestamp + ".png";

  try {
    if (!cv::imwrite(overlayFile, img)) {
      throw std::runtime_error("Failed to save overlay image to " +
                               overlayFile);
    }
  } catch (const cv::Exception &e) {
    throw std::runtime_error("OpenCV error while saving overlay: " +
                             std::string(e.what()));
  }

  return overlayFile;
}