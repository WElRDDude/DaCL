#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <string>
#include <vector>

class StorageManager {
private:
    std::string storagePath;

public:
    StorageManager(const std::string& path);
    void saveEventVideo(const std::vector<std::string>& chunks, const std::string& eventName);
};

#endif // STORAGE_MANAGER_H
