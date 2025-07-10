import RPi.GPIO as GPIO
from queue import Queue
import time

class ManualTriggerListener:
    def __init__(self, event_queue):
        self.event_queue = event_queue

    def start(self):
        GPIO.setmode(GPIO.BCM)
        BUTTON_PIN = 18
        GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

        while True:
            if GPIO.input(BUTTON_PIN) == GPIO.LOW:
                self.event_queue.put("MANUAL_EVENT")
                time.sleep(0.5)  # Debounce delay
            time.sleep(0.1)  # Small delay to reduce CPU usage
