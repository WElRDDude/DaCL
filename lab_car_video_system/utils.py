import os
import time
import datetime
import cv2
import numpy as np

def ensure_dir(directory):
    """Ensure that a directory exists, create if it doesn't."""
    if not os.path.exists(directory):
        os.makedirs(directory)

def get_current_timestamp():
    """Get current timestamp in a formatted string."""
    return datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

def generate_dummy_frame(width, height, timestamp, warning=None, speed=None):
    """Generate a dummy frame for testing."""
    frame = np.zeros((height, width, 3), dtype=np.uint8)

    # Add some dummy content to simulate a dashboard and road view
    cv2.putText(frame, "Dashboard View", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.putText(frame, "Road View", (50, height-50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

    return frame

def save_video_chunk(frames, filename):
    """Save a list of frames as a video file."""
    if not frames:
        return False

    height, width, _ = frames[0].shape
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(filename, fourcc, FPS, (width, height))

    for frame in frames:
        out.write(frame)

    out.release()
    return True
