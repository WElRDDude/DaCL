# Configuration settings for the lab car video system

# Video settings
VIDEO_WIDTH = 640
VIDEO_HEIGHT = 480
FPS = 30
CHUNK_DURATION_SECONDS = 30
VIDEO_CHUNK_SIZE = CHUNK_DURATION_SECONDS  # Assuming 30 seconds chunks

# Buffer settings
BUFFER_SIZE_MINUTES = 15
EVENT_WINDOW_MINUTES = 10

# File storage settings
VIDEO_STORAGE_DIR = 'video_chunks'
EVENT_VIDEO_DIR = 'event_videos'

# CAN Bus settings
CAN_INTERFACE = 'can0'
CAN_BUSTYPE = 'socketcan'

# Manual trigger settings
TRIGGER_GPIO_PIN = 17

# Overlay settings
OVERLAY_FONT_SCALE = 0.7
OVERLAY_FONT_THICKNESS = 2
OVERLAY_FONT_COLORS = {
    'timestamp': (0, 255, 0),    # Green
    'warning': (0, 0, 255),       # Red
    'speed': (255, 0, 0)          # Blue (but in BGR, it's actually red)
}
