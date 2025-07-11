import threading
import time
import configparser
import keyboard  # New import for keyboard input
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
        self.storage_manager = StorageManager(self.config)
        self.overlay_manager = OverlayManager()
        self.video_capture = VideoCapture(self.event_queue, self.storage_manager, self.overlay_manager, self.config)
        self.can_bus_listener = CanBusListener(self.event_queue, self.config)
        self.manual_trigger_listener = ManualTriggerListener(self.event_queue, self.config)

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

        # Start terminal input listener in a separate thread
        terminal_thread = threading.Thread(target=self.listen_for_terminal_input)
        terminal_thread.daemon = True
        terminal_thread.start()

        # Main loop
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            print("Shutting down gracefully...")
        finally:
            print("System shut down.")

    def listen_for_terminal_input(self):
        print("Terminal manual trigger enabled. Press 't' followed by Enter to trigger an event.")
        while True:
            try:
                # Wait for user input
                user_input = input()
                if user_input.lower() == 't':
                    print("Terminal trigger activated!")
                    self.event_queue.put("TERMINAL_EVENT")
            except KeyboardInterrupt:
                # Exit if Ctrl+C is pressed in the terminal
                break
            except EOFError:
                # Exit if input is closed (e.g., when running with input redirected)
                break

if __name__ == "__main__":
    # Install keyboard module if not already installed
    try:
        import keyboard
    except ImportError:
        print("Installing keyboard module...")
        import subprocess
        subprocess.check_call(["pip3", "install", "keyboard"])

    system = LabCarVideoSystem()
    system.start()
