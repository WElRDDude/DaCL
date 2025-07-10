import threading
import time
import configparser
from video_capture import VideoCapture
from can_bus import CanBusListener
from manual_trigger import ManualTriggerListener
from storage_manager import StorageManager
from overlay import OverlayManager
from queue import Queue

class LabCarVideoSystem:
    def __init__(self, config_file='config/config.ini'):
        self.config = configparser.ConfigParser()
        self.config.read(config_file)

        self.event_queue = Queue()
        self.storage_manager = StorageManager()
        self.overlay_manager = OverlayManager()
        self.video_capture = VideoCapture(self.event_queue, self.storage_manager, self.overlay_manager)
        self.can_bus_listener = CanBusListener(self.event_queue)
        self.manual_trigger_listener = ManualTriggerListener(self.event_queue)

    def start(self):
        # Start video capture thread
        video_thread = threading.Thread(target=self.video_capture.start)
        video_thread.daemon = True
        video_thread.start()

        # Start CAN bus listener thread
        can_thread = threading.Thread(target=self.can_bus_listener.start)
        can_thread.daemon = True
        can_thread.start()

        # Start manual trigger listener thread
        trigger_thread = threading.Thread(target=self.manual_trigger_listener.start)
        trigger_thread.daemon = True
        trigger_thread.start()

        # Main loop
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            print("Shutting down...")

if __name__ == "__main__":
    system = LabCarVideoSystem()
    system.start()
