#include "circular_buffer.hpp"

CircularBuffer::CircularBuffer(size_t max_size) : max_size_(max_size) {}

void CircularBuffer::add(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(filename);
    if (buffer_.size() > max_size_) {
        // Delete oldest file physically
        std::remove(buffer_.front().c_str());
        buffer_.pop_front();
    }
}

std::vector<std::string> CircularBuffer::get_last_n(size_t n) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> files;
    auto start = buffer_.size() > n ? buffer_.end() - n : buffer_.begin();
    for (auto it = start; it != buffer_.end(); ++it) {
        files.push_back(*it);
    }
    return files;
}

void CircularBuffer::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& file : buffer_) std::remove(file.c_str());
    buffer_.clear();
}