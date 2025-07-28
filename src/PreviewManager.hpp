/**
 * @file PreviewManager.hpp
 * @brief Live video preview functionality for development and monitoring
 */

#pragma once
#include <string>

/**
 * @class PreviewManager
 * @brief Provides live video preview capabilities using OpenCV display
 * 
 * This class enables real-time video monitoring by:
 * - Reading latest video segments from buffer directory
 * - Displaying video frames in an OpenCV window
 * - Providing visual feedback for development and debugging
 * 
 * @note Preview functionality is optional and typically used during development.
 *       Requires X11 forwarding or local display for GUI output.
 */
class PreviewManager final {
public:
    /**
     * @brief Constructs a PreviewManager for the specified buffer directory
     * @param bufferDir Directory containing video segments to preview
     * @throws std::invalid_argument if bufferDir is empty
     * @throws std::runtime_error if OpenCV display initialization fails
     */
    explicit PreviewManager(const std::string &bufferDir);
    
    /**
     * @brief Main loop for continuous video preview
     * @note This method runs indefinitely, displaying video frames.
     *       Should be executed in a separate thread.
     *       Press 'q' key to exit preview loop.
     */
    void run();

private:
    const std::string bufferDir_;  ///< Directory containing video segments to preview
    
    static constexpr int FRAME_DELAY_MS = 33;  ///< Delay between frames (~30 FPS)
};