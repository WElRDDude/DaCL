import cv2
import time
from threading import Lock
from datetime import datetime
from picamera import PiCamera
from picamera.array import PiRGBArray
import io
import numpy as np

class VideoCapture:
    def __init__(self, event_queue, storage_manager, overlay_manager, config):
        self.event_queue = event_queue
        self.storage_manager = storage_manager
        self.overlay_manager = overlay_manager
        self.video_buffer = []
        self.current_chunk = []
        self.chunk_start_time = time.time()
        self.buffer_lock = Lock()

        # Read configuration
        self.CHUNK_DURATION = int(config.get('video', 'chunk_duration'))
        self.BUFFER_DURATION = int(config.get('video', 'buffer_duration'))
        self.MAX_CHUNKS = self.BUFFER_DURATION // self.CHUNK_DURATION
        width, height = config.get('video', 'resolution').split('x')
        self.VIDEO_RESOLUTION = (int(width), int(height))
        self.VIDEO_FPS = int(config.get('video', 'fps'))
        self.is_recording_post_event = False
        self.post_event_chunks = []
        self.post_event_start_time = None

        # Initialize PiCamera
        self.camera = PiCamera()
        self.camera.resolution = self.VIDEO_RESOLUTION
        self.camera.framerate = self.VIDEO_FPS
        self.camera.awb_mode = 'auto'  # Auto white balance
        self.camera.exposure_mode = 'auto'
        # Give the camera some time to warm up
        time.sleep(2)

    def start(self):
        # Create a stream to capture video frames
        stream = io.BytesIO()

        try:
            for frame in self.camera.capture_continuous(stream, format='bgr', use_video_port=True, resize=self.VIDEO_RESOLUTION):
                # Convert the image from the stream to a numpy array
                data = np.frombuffer(stream.getvalue(), dtype=np.uint8)
                frame = cv2.imdecode(data, cv2.IMREAD_COLOR)
                stream.seek(0)
                stream.truncate()

                # Add timestamp and other overlays
                frame_with_overlay = self.overlay_manager.add_overlay(frame)

                # Add frame to current chunk
                self.current_chunk.append(frame_with_overlay)

                # Check if chunk duration has elapsed
                if time.time() - self.chunk_start_time >= self.CHUNK_DURATION:
                    with self.buffer_lock:
                        if self.current_chunk:  # Check if there are frames to add
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

        except Exception as e:
            print(f"An error occurred: {e}")
            self.camera.close()

    def handle_event(self):
        with self.buffer_lock:
            # Save the last 5 minutes (assuming 1-minute chunks, that's 5 chunks)
            chunks_to_save = []
            chunks_for_5_minutes = (5 * 60) // self.CHUNK_DURATION

            if len(self.video_buffer) >= chunks_for_5_minutes:
                chunks_to_save = self.video_buffer[-chunks_for_5_minutes:]
            else:
                chunks_to_save = self.video_buffer.copy()

            # Start recording post-event footage
            self.is_recording_post_event = True
            self.post_event_chunks = []
            self.post_event_start_time = time.time()

            # For now, just save the pre-event chunks
            # The post-event chunks will be saved when recording is done
            self.pre_event_chunks = chunks_to_save

    def save_event_video(self):
        # Combine pre-event and post-event chunks
        full_event_video = self.pre_event_chunks.copy()
        if self.post_event_chunks:
            full_event_video.append(self.post_event_chunks)

        # Generate filename with timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"event_{timestamp}.avi"

        # Save the video
        self.storage_manager.save_video(full_event_video, filename)

    def __del__(self):
        self.camera.close()
