import can
from queue import Queue
import time

class CanBusListener:
    def __init__(self, event_queue):
        self.event_queue = event_queue

    def start(self):
        bus = can.interface.Bus(channel='can0', bustype='socketcan')
        while True:
            message = bus.recv(timeout=1.0)  # Timeout to prevent busy waiting
            if message is not None:
                if self.is_event_trigger_message(message):
                    self.event_queue.put("CAN_EVENT")

    def is_event_trigger_message(self, message):
        # Example: Check if message ID is in a list of event-triggering IDs
        event_trigger_ids = [0x123, 0x456]  # Example IDs
        return message.arbitration_id in event_trigger_ids
