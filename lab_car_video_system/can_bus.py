import can
from queue import Queue
import time
import configparser

class CanBusListener:
    def __init__(self, event_queue, config):
        self.event_queue = event_queue
        self.config = config

    def start(self):
        bus = can.interface.Bus(channel=self.config.get('can_bus', 'interface'),
                               bustype=self.config.get('can_bus', 'bustype'))
        event_ids = [int(id.strip(), 16) for id in self.config.get('can_bus', 'event_ids').split(',')]

        while True:
            message = bus.recv(timeout=1.0)  # Timeout to prevent busy waiting
            if message is not None:
                if message.arbitration_id in event_ids:
                    self.event_queue.put("CAN_EVENT")

    def is_event_trigger_message(self, message):
        # This method is now handled by the start method with event_ids from config
        pass
