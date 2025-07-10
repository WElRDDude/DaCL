import cv2
from datetime import datetime

class OverlayManager:
    def __init__(self):
        self.last_speed = 0
        self.last_warning = ""

    def add_overlay(self, frame):
        # Add timestamp
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        cv2.putText(frame, timestamp, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        # Add speed (example)
        speed_text = f"Speed: {self.last_speed} km/h"
        cv2.putText(frame, speed_text, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        # Add warning if any
        if self.last_warning:
            cv2.putText(frame, self.last_warning, (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)

        return frame

    def update_speed(self, speed):
        self.last_speed = speed

    def update_warning(self, warning):
        self.last_warning = warning
