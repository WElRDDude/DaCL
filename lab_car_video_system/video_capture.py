import os
import time
import datetime
import threading
import numpy as np
import cv2
from picamera2 import Picamera2
from .config import *
from .utils import ensure_dir, get_current_timestamp, save_video_chunk
from .overlay import overlay_info_on_frame

class VideoChunk:
    def __init__(self, start_time):
        self.start_time = start_time
        self.end_time = None
        self.frames = []
        self.timestamp = None

class CircularBuffer:
    def __init__(self, max_size):
        self.buffer = []
        self.max_size = max_size
        self.lock = threading.Lock()

    def add_chunk(self, chunk):
        with self.lock:
            self.buffer.append(chunk)
            if len(self.buffer) > self.max_size:
                oldest_chunk = self.buffer.pop(0)
                # Optionally delete old chunks if they are stored on disk
                # os.remove(oldest_chunk)

    def get_chunks(self, start_time, end_time):
        relevant_chunks = []
        with self.lock:
            for chunk in self.buffer:
                chunk_time = chunk.start_time
                if start_time <= chunk_time <= end_time:
                    relevant_chunks.append(chunk)
        return relevant_chunks

    def clear(self):
        with self.lock:
            self.buffer = []

class VideoCaptureManager:
    def __init__(self):
        self.circular_buffer = CircularBuffer((BUFFER_SIZE_MINUTES * 60) // CHUNK_DURATION_SECONDS)
        self.current_chunk = None
        self.chunk_start_time = None
        self.chunk_frames = []
        self.camera = None
        self.stop_event = threading.Event()
        self.capture_thread = None
        ensure_dir(VIDEO_STORAGE_DIR)

    def start_capture(self):
        """Start the video capture thread."""
        self.stop_event.clear()
        self.capture_thread = threading.Thread(target=self._capture_loop)
        self.capture_thread.start()

    def stop_capture(self):
        """Stop the video capture thread."""
        self.stop_event.set()
        if self.capture_thread:
            self.capture_thread.join()
        if self.camera and hasattr(self.camera, 'stop'):
            self.camera.stop()

    def _capture_loop(self):
        """Main video capture loop using libcamera."""
        try:
            # Initialize the camera using libcamera
            self.camera = Picamera2()
            camera_config = self.camera.create_video_configuration(
                main={"format": 'XRGB8888', "size": (VIDEO_WIDTH, VIDEO_HEIGHT)},
                controls={"FrameRate": FPS}
            )
            self.camera.configure(camera_config)
            self.camera.start()

            chunk_duration = CHUNK_DURATION_SECONDS
            chunk_frames_count = chunk_duration * FPS
            frame_count = 0
            self.chunk_start_time = time.time()
            self.current_chunk = VideoChunk(self.chunk_start_time)

            while not self.stop_event.is_set():
                timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                warning = None
                speed = None

                # Capture frame
                frame = self.camera.capture_array()
                if frame is not None:
                    # Convert frame to BGR format, which OpenCV expects
                    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
                    # Overlay information on the frame
                    frame = overlay_info_on_frame(frame, timestamp, warning, speed)
                    self.chunk_frames.append(frame)
                    frame_count += 1

                    if frame_count >= chunk_frames_count:
                        self._save_current_chunk()
                        frame_count = 0

                # Simulate frame rate delay
                time.sleep(1/FPS)

        finally:
            if self.camera:
                self.camera.stop()

        # Save any remaining frames in the current chunk
        if self.chunk_frames:
            self._save_current_chunk()

    def _save_current_chunk(self):
        """Save the current chunk to the circular buffer."""
        if not self.chunk_frames:
            return

        self.current_chunk.frames = self.chunk_frames.copy()
        self.current_chunk.end_time = time.time()
        self.current_chunk.timestamp = time.time()

        # Add the chunk to the circular buffer
        self.circular_buffer.add_chunk(self.current_chunk)

        # Reset for the next chunk
        self.chunk_frames = []
        self.chunk_start_time = time.time()
        self.current_chunk = VideoChunk(self.chunk_start_time)

    def get_event_video(self, event_time):
        """Retrieve video chunks around an event time."""
        start_time = event_time - 5 * 60  # 5 minutes before the event
        end_time = event_time + 5 * 60    # 5 minutes after the event

        relevant_chunks = self.circular_buffer.get_chunks(start_time, end_time)
        return relevant_chunks

    def save_event_video(self, event_time):
        """Save the event video to disk."""
        chunks = self.get_event_video(event_time)
        if not chunks:
            return False

        # Create a unique filename for the event video
        timestamp = get_current_timestamp()
        event_video_path = os.path.join(EVENT_VIDEO_DIR, f"event_{timestamp}.avi")

        # Combine all frames from the chunks into one video
        all_frames = []
        for chunk in chunks:
            all_frames.extend(chunk.frames)

        if all_frames:
            ensure_dir(EVENT_VIDEO_DIR)
            return save_video_chunk(all_frames, event_video_path)

        return False
