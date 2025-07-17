#pragma once
#include <deque>
#include <string>
#include <mutex>
#include <vector>

class CircularBuffer {
public:
    CircularBuffer(size_t max_size);
    void add(const std::string& filename);
    std::vector<std::string> get_last_n(size_t n) const;
    void cleanup();

private:
    std::deque<std::string> buffer_;
    size_t max_size_;
    mutable std::mutex mutex_;
};