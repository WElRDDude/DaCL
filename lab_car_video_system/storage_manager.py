import cv2
import os
from datetime import datetime

class StorageManager:
    def __init__(self):
        self.storage_dir = "event_videos"
        if not os.path.exists(self.storage_dir):
            os.makedirs(self.storage_dir)

    def save_video(self, video_chunks, filename):
        # Combine all chunks into one video
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        # Assuming all chunks have the same resolution and FPS
        height, width, _ = video_chunks[0][0].shape if len(video_chunks) > 0 and len(video_chunks[0]) > 0 else (480, 640, 3)
        out = cv2.VideoWriter(os.path.join(self.storage_dir, filename), fourcc, 30.0, (width, height))

        for chunk in video_chunks:
            for frame in chunk:
                out.write(frame)

        out.release()
        print(f"Saved video: {filename}")

    def cleanup_old_videos(self, max_days=30):
        # Implement logic to delete videos older than max_days
        pass
