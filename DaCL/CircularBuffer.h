#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <vector>
#include <string>
#include <filesystem>
#include <mutex>

class CircularBuffer {
private:
    std::vector<std::string> buffer;
    size_t maxSize;
    size_t currentSize;
    std::mutex bufferMutex;

public:
    CircularBuffer(size_t size);
    void addChunk(const std::string& chunkPath);
    std::vector<std::string> getChunks(int startIndex, int count);
    void clear();
    size_t getCurrentSize() const { return currentSize; }
};

#endif // CIRCULAR_BUFFER_H
