#include "CANListener.hpp"
#include "CSVLogger.hpp"
#include "FileManager.hpp"
#include "OverlayRenderer.hpp"
#include "PreviewManager.hpp"
#include "StorageManager.hpp"
#include "TriggerManager.hpp"
#include "VideoRecorder.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <thread>

int main(int argc, char *argv[]) {
  bool enablePreview = (argc > 1 && std::string(argv[1]) == "--preview");

  Config config("configs/config.ini");
  auto idToWarning = parseCANWarnings(config.warningIds);

  std::filesystem::create_directories(config.bufferDir);
  std::filesystem::create_directories(config.eventDir);
  std::filesystem::create_directories("logs");

  CANListener canListener(config.canIface, idToWarning);
  VideoRecorder videoRecorder(config.bufferDir, config.segmentSeconds,
                              config.bufferMinutes, &canListener);
  OverlayRenderer overlayRenderer(&canListener); // Pass CANListener instance
  FileManager fileManager(config.bufferDir, config.eventDir);
  CSVLogger csvLogger("logs/events.csv");
  TriggerManager triggerManager(
      &videoRecorder, &fileManager, &csvLogger, &overlayRenderer, &canListener,
      config.buttonPin, config.pretriggerMinutes, config.posttriggerMinutes);
  StorageManager storageManager(config.bufferDir, config.bufferMinutes + 2);

  std::thread videoThread(&VideoRecorder::run, &videoRecorder);
  std::thread triggerThread(&TriggerManager::run, &triggerManager);
  std::thread canThread(&CANListener::run, &canListener);
  std::thread storageThread(&StorageManager::run, &storageManager);

  std::thread previewThread;
  if (enablePreview) {
    PreviewManager previewManager(config.bufferDir);
    previewThread = std::thread(&PreviewManager::run, &previewManager);
  }

  videoThread.join();
  triggerThread.join();
  canThread.join();
  storageThread.join();
  if (enablePreview)
    previewThread.join();

  return 0;
}