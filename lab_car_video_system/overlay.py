import cv2
import datetime
from .config import *

def overlay_info_on_frame(frame, timestamp, warning=None, speed=None):
    """
    Overlay timestamp, warning message, and speed on a video frame.

    Args:
        frame: The video frame to overlay on
        timestamp: The timestamp to display
        warning: The warning message to display (optional)
        speed: The vehicle speed to display (optional)

    Returns:
        The frame with overlays applied
    """
    # Overlay timestamp
    cv2.putText(frame, f"Time: {timestamp}", (50, 100), cv2.FONT_HERSHEY_SIMPLEX,
                OVERLAY_FONT_SCALE, OVERLAY_FONT_COLORS['timestamp'], OVERLAY_FONT_THICKNESS)

    # Overlay warning if present
    if warning:
        cv2.putText(frame, f"Warning: {warning}", (50, 150), cv2.FONT_HERSHEY_SIMPLEX,
                    OVERLAY_FONT_SCALE, OVERLAY_FONT_COLORS['warning'], OVERLAY_FONT_THICKNESS)

    # Overlay speed if present
    if speed is not None:
        cv2.putText(frame, f"Speed: {speed} km/h", (50, 200), cv2.FONT_HERSHEY_SIMPLEX,
                    OVERLAY_FONT_SCALE, OVERLAY_FONT_COLORS['speed'], OVERLAY_FONT_THICKNESS)

    return frame
