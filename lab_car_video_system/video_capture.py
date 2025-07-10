import cv2
import time
from threading import Lock
from datetime import datetime
from picamera import PiCamera
from picamera.array import PiRGBArray
import io
import numpy as np

class VideoCapture:
    def __init__(self, event_queue, storage_manager, overlay_manager):
        self.event_queue = event_queue
        self.storage_manager = storage_manager
        self.overlay_manager = overlay_manager
        self.video_buffer = []
        self.current_chunk = []
        self.chunk_start_time = time.time()
        self.buffer_lock = Lock()
        self.CHUNK_DURATION = 60  # seconds (1 minute)
        self.BUFFER_DURATION = 15 * 60  # seconds (15 minutes)
        self.MAX_CHUNKS = self.BUFFER_DURATION // self.CHUNK_DURATION  # 15 chunks
        self.VIDEO_RESOLUTION = (640, 480)
        self.VIDEO_FPS = 30
        self.is_recording_post_event = False
        self.post_event_chunks = []
        self.post_event_start_time = None

        # Initialize PiCamera
        self.camera = PiCamera()
        self.camera.resolution = self.VIDEO_RESOLUTION
        self.camera.framerate = self.VIDEO_FPS
        # Give the camera some time to warm up
        time.sleep(2)

    def start(self):
        # Create a stream to capture video frames
        stream = io.BytesIO()

        for frame in self.camera.capture_continuous(stream, format='bgr', use_video_port=True):
            # Convert the image from the stream to a numpy array
            data = np.frombuffer(stream.getvalue(), dtype=np.uint8)
            frame = cv2.imdecode(data, 1)
            stream.seek(0)
            stream.truncate()

            # Add timestamp and other overlays
            frame_with_overlay = self.overlay_manager.add_overlay(frame)

            # Add frame to current chunk
            self.current_chunk.append(frame_with_overlay)

            # Check if chunk duration has elapsed
            if time.time() - self.chunk_start_time >= self.CHUNK_DURATION:
                with self.buffer_lock:
                    self.video_buffer.append(self.current_chunk.copy())
                    self.current_chunk = []
                    self.chunk_start_time = time.time()

                    # Maintain buffer size
                    if len(self.video_buffer) > self.MAX_CHUNKS:
                        self.video_buffer.pop(0)  # Remove oldest chunk

            # Check for events in the queue
            if not self.event_queue.empty():
                event = self.event_queue.get()
                self.handle_event()

            # If recording post-event footage, check if time is up
            if self.is_recording_post_event:
                if time.time() - self.post_event_start_time >= 300:  # 5 minutes
                    self.save_event_video()
                    self.is_recording_post_event = False
                    self.post_event_chunks = []
