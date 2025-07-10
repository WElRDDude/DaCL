import threading
import time
import random
import can
from .config import CAN_INTERFACE, CAN_BUSTYPE

class CANBusListener:
    def __init__(self, trigger_callback):
        self.trigger_callback = trigger_callback
        self.stop_event = threading.Event()
        self.listen_thread = None
        self.bus = None

    def start_listening(self):
        """Start the CAN bus listening thread."""
        self.stop_event.clear()
        self.listen_thread = threading.Thread(target=self._listen_loop)
        self.listen_thread.start()

    def stop_listening(self):
        """Stop the CAN bus listening thread."""
        self.stop_event.set()
        if self.listen_thread:
            self.listen_thread.join()
        if self.bus and hasattr(self.bus, 'shutdown'):
            self.bus.shutdown()

    def _listen_loop(self):
        """Main CAN bus listening loop."""
        try:
            # Try to initialize real CAN bus interface
            try:
                self.bus = can.interface.Bus(channel=CAN_INTERFACE, bustype=CAN_BUSTYPE)
                print("Connected to CAN bus")
                self._real_can_listen()
            except Exception as e:
                print(f"Could not initialize CAN bus: {e}")
                print("Falling back to simulation mode")
                self._simulate_can_messages()

        except Exception as e:
            print(f"Error in CAN bus listener: {e}")

    def _real_can_listen(self):
        """Listen to real CAN bus messages."""
        while not self.stop_event.is_set():
            message = self.bus.recv(timeout=1.0)
            if message is not None:
                # Process the CAN message
                # In a real implementation, you would parse the message
                # to detect warnings and extract vehicle speed
                print(f"Received CAN message: {message}")

                # For now, we'll just trigger on any message
                self.trigger_callback()

    def _simulate_can_messages(self):
        """Simulate CAN bus messages for testing."""
        while not self.stop_event.is_set():
            # Simulate a warning message every 30-60 seconds
            time.sleep(random.uniform(30, 60))
            warning_messages = ["Engine Fault", "Brake Warning", "Low Oil", "Check Engine", "ABS Fault"]
            warning = random.choice(warning_messages)
            print(f"CAN Warning Detected: {warning}")
            self.trigger_callback()
