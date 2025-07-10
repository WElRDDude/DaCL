import cv2
import time
from threading import Lock
from datetime import datetime

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

    def start(self):
        cap = cv2.VideoCapture(0)  # Assuming camera index 0
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.VIDEO_RESOLUTION[0])
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.VIDEO_RESOLUTION[1])
        cap.set(cv2.CAP_PROP_FPS, self.VIDEO_FPS)

        while True:
            ret, frame = cap.read()
            if not ret:
                break

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
        full_event_video = self.pre_event_chunks + [self.post_event_chunks]
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"event_{timestamp}.avi"
        self.storage_manager.save_video(full_event_video, filename)
