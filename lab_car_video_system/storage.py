import os
import time
import datetime
from .config import VIDEO_STORAGE_DIR, EVENT_VIDEO_DIR
from .utils import ensure_dir, get_current_timestamp, save_video_chunk

class VideoStorageManager:
    def __init__(self):
        ensure_dir(VIDEO_STORAGE_DIR)
        ensure_dir(EVENT_VIDEO_DIR)
        self.cleanup_old_chunks()

    def save_chunk(self, chunk, chunk_id):
        """Save a video chunk to disk."""
        filename = os.path.join(VIDEO_STORAGE_DIR, f"chunk_{chunk_id}.avi")
        if chunk.frames:
            return save_video_chunk(chunk.frames, filename)
        return False

    def save_event_video(self, chunks, event_time):
        """Save an event video to disk."""
        timestamp = datetime.datetime.fromtimestamp(event_time).strftime("%Y-%m-%d_%H-%M-%S")
        filename = os.path.join(EVENT_VIDEO_DIR, f"event_{timestamp}.avi")

        # Combine all frames from the chunks into one video
        all_frames = []
        for chunk in chunks:
            all_frames.extend(chunk.frames)

        if all_frames:
            return save_video_chunk(all_frames, filename)

        return False

    def cleanup_old_chunks(self, keep_duration_minutes=BUFFER_SIZE_MINUTES):
        """Clean up old video chunks that are no longer needed."""
        now = time.time()
        cutoff_time = now - (keep_duration_minutes * 60)

        for filename in os.listdir(VIDEO_STORAGE_DIR):
            filepath = os.path.join(VIDEO_STORAGE_DIR, filename)
            if os.path.isfile(filepath):
                file_time = os.path.getmtime(filepath)
                if file_time < cutoff_time:
                    try:
                        os.remove(filepath)
                    except:
                        pass
