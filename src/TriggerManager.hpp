/**
 * @file TriggerManager.hpp
 * @brief Event trigger coordination and processing system
 */

#pragma once
#include "VideoRecorder.hpp"
#include "FileManager.hpp"
#include "CSVLogger.hpp"
#include "OverlayRenderer.hpp"
#include "CANListener.hpp"
#include <atomic>

/**
 * @class TriggerManager
 * @brief Coordinates event triggers from multiple sources and manages event processing
 * 
 * This class serves as the central coordinator for event handling:
 * - Monitors CAN bus for warning messages
 * - Handles GPIO button press events
 * - Processes console-based manual triggers
 * - Coordinates video segment saving and overlay application
 * - Manages event logging and metadata recording
 * 
 * @note Thread Safety: This class manages multiple trigger sources and coordinates
 *       with other system components in a thread-safe manner.
 */
class TriggerManager final {
public:
    /**
     * @brief Constructs a TriggerManager with system component references
     * @param videoRecorder Pointer to video recording system
     * @param fileManager Pointer to file management system
     * @param csvLogger Pointer to event logging system
     * @param overlayRenderer Pointer to overlay generation system
     * @param canListener Pointer to CAN bus interface
     * @param gpioPin GPIO pin number for manual trigger button
     * @param preMin Pre-trigger duration in minutes
     * @param postMin Post-trigger duration in minutes
     * @throws std::invalid_argument if any pointer is nullptr or timing parameters are invalid
     */
    explicit TriggerManager(VideoRecorder* videoRecorder,
                           FileManager* fileManager,
                           CSVLogger* csvLogger,
                           OverlayRenderer* overlayRenderer,
                           CANListener* canListener,
                           int gpioPin,
                           int preMin,
                           int postMin);
    
    /**
     * @brief Main event monitoring and processing loop
     * @note This method runs indefinitely, monitoring all trigger sources.
     *       Should be executed in a separate thread.
     */
    void run();

private:
    /**
     * @brief Processes GPIO button press events
     * @note Called internally during main monitoring loop
     */
    void handleGPIOTrigger();
    
    /**
     * @brief Processes CAN bus warning message events
     * @note Called internally during main monitoring loop
     */
    void handleCANTrigger();
    
    /**
     * @brief Processes console-based manual trigger commands
     * @note Called internally during main monitoring loop
     */
    void handleConsoleTrigger();
    
    // System component pointers - all non-owning
    VideoRecorder* const videoRecorder_;     ///< Video recording system
    FileManager* const fileManager_;         ///< File management system
    CSVLogger* const csvLogger_;             ///< Event logging system
    OverlayRenderer* const overlayRenderer_; ///< Overlay generation system
    CANListener* const canListener_;         ///< CAN bus interface
    
    const int gpioPin_;    ///< GPIO pin for manual trigger button
    const int preMin_;     ///< Pre-trigger duration in minutes
    const int postMin_;    ///< Post-trigger duration in minutes
    
    std::atomic<bool> running_;  ///< Flag controlling main processing loop
    
    static constexpr int POLLING_INTERVAL_MS = 100;  ///< Main loop polling interval
};