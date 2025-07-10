import os
import sys
import signal
import threading
import time
from .video_capture import VideoCaptureManager
from .can_bus import CANBusListener
from .manual_trigger import ManualTrigger
from .storage import VideoStorageManager

class LabCarVideoSystem:
    def __init__(self):
        self.video_capture = VideoCaptureManager()
        self.storage = VideoStorageManager()
        self.can_bus_listener = None
        self.manual_trigger = None
        self.running = False

    def start(self):
        """Start the entire system."""
        self.running = True

        # Start video capture
        self.video_capture.start_capture()

        # Setup CAN bus listener
        self.can_bus_listener = CANBusListener(self.handle_trigger_event)
        self.can_bus_listener.start_listening()

        # Setup manual trigger
        self.manual_trigger = ManualTrigger(self.handle_trigger_event)
        self.manual_trigger.start_listening()

        print("Lab car video system started. Press Ctrl+C to stop.")

        # Keep the main thread running
        try:
            while self.running:
                time.sleep(1)
        except KeyboardInterrupt:
            self.stop()

    def stop(self):
        """Stop the entire system."""
        self.running = False
        print("\nStopping lab car video system...")

        # Stop all components
        if self.video_capture:
            self.video_capture.stop_capture()
        if self.can_bus_listener:
            self.can_bus_listener.stop_listening()
        if self.manual_trigger:
            self.manual_trigger.stop_listening()

        print("System stopped.")

    def handle_trigger_event(self):
        """Handle a trigger event (from CAN bus or manual trigger)."""
        print("Trigger event detected!")
        event_time = time.time()

        # Get the relevant video chunks and save them
        self.video_capture.save_event_video(event_time)

def main():
    system = LabCarVideoSystem()
    system.start()

if __name__ == "__main__":
    main()
