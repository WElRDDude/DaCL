/**
 * @file FileManager.hpp
 * @brief File operations manager for video segment handling and event archival
 */

#pragma once
#include <string>
#include <vector>

/**
 * @class FileManager
 * @brief Manages file operations for video segments and event archival
 *
 * This class handles:
 * - Copying video segments from buffer to event directory
 * - Applying overlays to video files using ffmpeg
 * - Cleaning up old video segments to maintain storage limits
 * - File naming conventions for event videos
 *
 * @note This class performs file I/O operations and may throw filesystem
 * exceptions
 */
class FileManager final {
public:
  /**
   * @brief Constructs a FileManager with specified directories
   * @param bufferDir Source directory containing buffered video segments
   * @param eventDir Destination directory for saved event videos
   * @throws std::invalid_argument if either directory path is empty
   * @throws std::filesystem::filesystem_error if directories cannot be accessed
   */
  explicit FileManager(const std::string &bufferDir,
                       const std::string &eventDir);

  /**
   * @brief Copies and processes video segments for event archival
   * @param segments List of video segment filenames to copy
   * @param warningType Type of warning that triggered the event
   * @param timestamp Formatted timestamp for file naming (YYYYMMDD_HHMMSS)
   * @param overlayFile Path to overlay image file for video annotation
   * @param suffix File naming suffix ("pretrigger" or "posttrigger")
   * @throws std::runtime_error if copy operations fail
   * @throws std::invalid_argument if any parameter is empty
   *
   * @note Uses ffmpeg subprocess to apply overlays to copied video files
   */
  void copyEventSegments(const std::vector<std::string> &segments,
                         const std::string &warningType,
                         const std::string &timestamp,
                         const std::string &overlayFile,
                         const std::string &suffix);

  /**
   * @brief Removes old video segments from buffer directory
   * @param maxMinutes Maximum age of segments to keep (in minutes)
   * @throws std::filesystem::filesystem_error if directory operations fail
   *
   * @note This method is typically called by StorageManager to maintain buffer
   * size
   */
  void cleanOldSegments(int maxMinutes);

private:
  const std::string
      bufferDir_;              ///< Source directory for buffered video segments
  const std::string eventDir_; ///< Destination directory for event videos
};