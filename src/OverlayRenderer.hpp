/**
 * @file OverlayRenderer.hpp
 * @brief OpenCV-based overlay generation for video annotation
 */

#pragma once
#include "CANListener.hpp"
#include <string>

/**
 * @class OverlayRenderer
 * @brief Generates overlay images for video annotation with real-time data
 *
 * This class creates overlay images containing:
 * - Vehicle speed information
 * - Warning type indicators
 * - Timestamp information
 * - Additional CAN-derived data
 *
 * Generated overlays are used by FFmpeg to annotate video files during event
 * processing.
 *
 * @note Requires OpenCV for image generation and text rendering
 */
class OverlayRenderer final {
public:
  /**
   * @brief Constructs an OverlayRenderer with CAN data integration
   * @param canListener Pointer to CAN listener for real-time data access
   * @throws std::invalid_argument if canListener is nullptr
   */
  explicit OverlayRenderer(CANListener *canListener);

  /**
   * @brief Generates an overlay image with specified information
   * @param speed Vehicle speed to display (overrides CAN data if != -1)
   * @param warningType Warning message to display
   * @param timestamp Timestamp string for display
   * @return Path to generated overlay image file
   * @throws std::runtime_error if image generation fails
   *
   * @note Generated overlay files are temporary and should be cleaned up after
   * use
   */
  std::string renderOverlay(int speed, const std::string &warningType,
                            const std::string &timestamp);

private:
  CANListener *const canListener_; ///< Pointer to CAN listener for data access

  // Overlay styling constants
  static constexpr int OVERLAY_WIDTH = 800;  ///< Overlay image width
  static constexpr int OVERLAY_HEIGHT = 200; ///< Overlay image height
  static constexpr int FONT_SCALE = 2;       ///< Text font scale factor
  static constexpr int TEXT_THICKNESS = 3;   ///< Text line thickness
};