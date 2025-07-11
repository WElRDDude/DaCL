import RPi.GPIO as GPIO
from queue import Queue
import time
import configparser

class ManualTriggerListener:
    def __init__(self, event_queue, config):
        self.event_queue = event_queue
        self.config = config
        self.BUTTON_PIN = int(self.config.get('manual_trigger', 'button_pin'))
        self.DEBOUNCE_DELAY = float(self.config.get('manual_trigger', 'debounce_delay'))

    def start(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

        while True:
            if GPIO.input(self.BUTTON_PIN) == GPIO.LOW:
                self.event_queue.put("MANUAL_EVENT")
                time.sleep(self.DEBOUNCE_DELAY)  # Debounce delay
            time.sleep(0.1)  # Small delay to reduce CPU usage
