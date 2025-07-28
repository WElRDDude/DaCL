/**
 * @file VideoRecorder.hpp
 * @brief Core video recording engine with segmented buffering and event capture
 */

#pragma once
#include "CANListener.hpp"
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

/**
 * @class VideoRecorder
 * @brief Manages continuous video recording with circular buffering and event
 * capture
 *
 * This class provides the core video recording functionality:
 * - Continuous segmented recording to maintain a rolling buffer
 * - Event-triggered video capture with pre/post trigger periods
 * - Thread-safe access to buffered video segments
 * - Integration with CAN data for timestamping and metadata
 *
 * @note Thread Safety: All public methods are thread-safe using internal
 * mutexes. The run() method should be executed in a dedicated thread.
 */
class VideoRecorder final {
public:
  /**
   * @brief Constructs a VideoRecorder with specified parameters
   * @param bufferDir Directory for storing video segment buffer
   * @param segmentSeconds Duration of each video segment in seconds
   * @param bufferMinutes Total buffer duration in minutes
   * @param canListener Pointer to CAN listener for metadata integration
   * @throws std::invalid_argument if any parameter is invalid
   * @throws std::runtime_error if video capture initialization fails
   */
  explicit VideoRecorder(const std::string &bufferDir, int segmentSeconds,
                         int bufferMinutes, CANListener *canListener);

  /**
   * @brief Main recording loop for continuous video capture
   * @note This method runs indefinitely, creating video segments.
   *       Should be executed in a separate thread.
   */
  void run();

  /**
   * @brief Retrieves list of buffered video segments for event processing
   * @param minutesBack Number of minutes of segments to retrieve
   * @return Vector of segment filenames sorted chronologically
   * @note Thread-safe: Can be called from trigger processing threads
   */
  std::vector<std::string> getBufferedSegments(int minutesBack);

  /**
   * @brief Initiates post-trigger recording for event capture
   * @param minutesForward Duration of post-trigger recording in minutes
   * @param eventType Type of event triggering the recording
   * @param[out] postFileOut Filename of the post-trigger video file
   * @note Thread-safe: Coordinates with main recording loop
   */
  void startPostTriggerRecording(int minutesForward,
                                 const std::string &eventType,
                                 std::string &postFileOut);

private:
  const std::string bufferDir_; ///< Directory for video segment storage
  const int segmentSeconds_;    ///< Duration per segment in seconds
  const int bufferMinutes_;     ///< Total buffer duration in minutes

  std::vector<std::string> bufferFiles_; ///< List of current buffer files
  std::mutex mtx_;                       ///< Mutex for thread synchronization
  std::condition_variable cv_; ///< Condition variable for coordination

  // Post-trigger recording state
  bool postTriggerActive_;      ///< Flag indicating post-trigger recording in
                                ///< progress
  int postTriggerSegmentsLeft_; ///< Remaining segments for post-trigger
                                ///< recording
  std::string eventType_;       ///< Current event type being recorded
  std::string postTriggerFile_; ///< Output filename for post-trigger video

  CANListener *const canListener_; ///< Pointer to CAN listener for metadata

  static constexpr int MAX_BUFFER_FILES =
      60; ///< Maximum files in circular buffer
};