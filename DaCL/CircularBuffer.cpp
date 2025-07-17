#include "CircularBuffer.h"
#include <iostream>
#include <filesystem>

CircularBuffer::CircularBuffer(size_t size) : maxSize(size), currentSize(0) {
    buffer.reserve(maxSize);

    // Delete old chunk files from previous runs
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            auto path = entry.path();
            if (path.extension() == ".mp4" || path.extension() == ".h264") {
                std::filesystem::remove(path);
            }
        }
    }
}

void CircularBuffer::addChunk(const std::string& chunkPath) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    if (currentSize >= maxSize) {
        std::filesystem::remove(buffer[0]);
        buffer.erase(buffer.begin());
        currentSize--;
    }
    buffer.push_back(chunkPath);
    currentSize++;
}

std::vector<std::string> CircularBuffer::getChunks(int startIndex, int count) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    std::vector<std::string> result;
    if (startIndex < 0) startIndex = 0;
    if (startIndex >= currentSize) return result;
    for (int i = startIndex; i < startIndex + count && i < currentSize; i++) {
        result.push_back(buffer[i]);
    }
    return result;
}

void CircularBuffer::clear() {
    std::lock_guard<std::mutex> lock(bufferMutex);
    for (const auto& chunk : buffer) {
        std::filesystem::remove(chunk);
    }
    buffer.clear();
    currentSize = 0;
}
