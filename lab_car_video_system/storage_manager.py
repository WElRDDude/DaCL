import cv2
import os
from datetime import datetime
import configparser

class StorageManager:
    def __init__(self, config):
        self.config = config
        self.storage_dir = self.config.get('storage', 'storage_dir')
        if not os.path.exists(self.storage_dir):
            os.makedirs(self.storage_dir)

    def save_video(self, video_chunks, filename):
        # Combine all chunks into one video
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        # Assuming all chunks have the same resolution and FPS
        if len(video_chunks) > 0 and len(video_chunks[0]) > 0:
            height, width, _ = video_chunks[0][0].shape
            out 
