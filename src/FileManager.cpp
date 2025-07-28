#include "FileManager.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>

FileManager::FileManager(const std::string &bufferDir,
                         const std::string &eventDir)
    : bufferDir_(bufferDir), eventDir_(eventDir) {
  // Input validation
  if (bufferDir.empty()) {
    throw std::invalid_argument("Buffer directory path cannot be empty");
  }
  if (eventDir.empty()) {
    throw std::invalid_argument("Event directory path cannot be empty");
  }

  // Verify directory accessibility
  std::error_code ec;
  if (!std::filesystem::exists(bufferDir_, ec) &&
      !std::filesystem::create_directories(bufferDir_, ec)) {
    throw std::filesystem::filesystem_error(
        "Cannot access or create buffer directory", bufferDir_, ec);
  }
  if (!std::filesystem::exists(eventDir_, ec) &&
      !std::filesystem::create_directories(eventDir_, ec)) {
    throw std::filesystem::filesystem_error(
        "Cannot access or create event directory", eventDir_, ec);
  }
}

void FileManager::copyEventSegments(const std::vector<std::string> &segments,
                                    const std::string &warningType,
                                    const std::string &timestamp,
                                    const std::string &overlayFile,
                                    const std::string &suffix) {
  // Input validation
  if (segments.empty()) {
    throw std::invalid_argument("Segments list cannot be empty");
  }
  if (warningType.empty()) {
    throw std::invalid_argument("Warning type cannot be empty");
  }
  if (timestamp.empty()) {
    throw std::invalid_argument("Timestamp cannot be empty");
  }
  if (overlayFile.empty()) {
    throw std::invalid_argument("Overlay file path cannot be empty");
  }
  if (suffix.empty()) {
    throw std::invalid_argument("Suffix cannot be empty");
  }

  // Ensure the event directory exists
  std::error_code ec;
  if (!std::filesystem::create_directories(eventDir_, ec) && ec) {
    throw std::runtime_error("Failed to create event directory: " +
                             ec.message());
  }

  for (size_t i = 0; i < segments.size(); ++i) {
    const std::string dest = eventDir_ + "/" + timestamp + "_" + warningType +
                             "_" + suffix + "_" + std::to_string(i) + ".mp4";

    // Copy file with error checking
    try {
      std::filesystem::copy(segments[i], dest,
                            std::filesystem::copy_options::overwrite_existing);
    } catch (const std::filesystem::filesystem_error &e) {
      throw std::runtime_error("Failed to copy segment " + segments[i] +
                               " to " + dest + ": " + e.what());
    }

    // Apply overlay using ffmpeg - improved command construction and error
    // handling
    const std::string tempDest = dest + "_temp.mp4";
    const std::string cmd =
        "ffmpeg -y -i " + dest + " -vf \"movie=" + overlayFile +
        " [overlay]; [in][overlay] overlay=0:0 [out]\" -codec:a copy " +
        tempDest + " && mv " + tempDest + " " + dest;

    const int ret = std::system(cmd.c_str());
    if (ret != 0) {
      std::cerr << "Warning: ffmpeg overlay command failed for " << dest
                << " (return code: " << ret << ")" << std::endl;
      // Continue processing other segments even if overlay fails
    }
  }
}

void FileManager::cleanOldSegments(int maxMinutes) {
  // Input validation
  if (maxMinutes <= 0) {
    throw std::invalid_argument("maxMinutes must be positive");
  }

  using namespace std::chrono;
  const auto now = system_clock::now();

  std::error_code ec;
  if (!std::filesystem::exists(bufferDir_, ec)) {
    if (ec) {
      throw std::filesystem::filesystem_error("Cannot access buffer directory",
                                              bufferDir_, ec);
    }
    return; // Directory doesn't exist, nothing to clean
  }

  for (const auto &entry :
       std::filesystem::directory_iterator(bufferDir_, ec)) {
    if (ec) {
      std::cerr << "Warning: Error iterating directory " << bufferDir_ << ": "
                << ec.message() << std::endl;
      continue;
    }

    if (!entry.is_regular_file()) {
      continue; // Skip non-regular files
    }

    try {
      const auto ftime = std::filesystem::last_write_time(entry);
      const auto sctp =
          std::chrono::time_point_cast<std::chrono::system_clock::duration>(
              ftime - std::filesystem::file_time_type::clock::now() +
              std::chrono::system_clock::now());
      const auto age =
          std::chrono::duration_cast<std::chrono::minutes>(now - sctp);

      if (age.count() > maxMinutes) {
        std::filesystem::remove(entry, ec);
        if (ec) {
          std::cerr << "Warning: Failed to remove old segment " << entry.path()
                    << ": " << ec.message() << std::endl;
        }
      }
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Warning: Error processing file " << entry.path() << ": "
                << e.what() << std::endl;
    }
  }
}