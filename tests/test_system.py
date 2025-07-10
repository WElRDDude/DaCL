import os
import time
import threading
import random
import datetime
import cv2
import numpy as np
import sys
import select

# Constants
BUFFER_SIZE_MINUTES = 15
EVENT_WINDOW_MINUTES = 10
CHUNK_DURATION_SECONDS = 30
VIDEO_CHUNK_SIZE = CHUNK_DURATION_SECONDS  # Assuming 30 seconds chunks
VIDEO_WIDTH, VIDEO_HEIGHT = 640, 480  # Standard video resolution for testing
FPS = 30  # Frames per second

# Simulated CAN messages
class CANMessage:
    def __init__(self, warning=None, speed=None):
        self.warning = warning
        self.speed = speed

# Circular Buffer Implementation
class CircularBuffer:
    def __init__(self, max_size):
        self.buffer = []
        self.max_size = max_size

    def add_chunk(self, chunk):
        self.buffer.append(chunk)
        if len(self.buffer) > self.max_size:
            oldest_chunk = self.buffer.pop(0)
            # In a real scenario, you might want to delete the file here
            # os.remove(oldest_chunk)

    def get_chunks(self, start_time, end_time):
        # Logic to retrieve chunks within the given time window
        relevant_chunks = []
        for chunk in self.buffer:
            chunk_time = chunk['timestamp']
            if start_time <= chunk_time <= end_time:
                relevant_chunks.append(chunk)
        return relevant_chunks

# Video Recording Simulation
def generate_dummy_frame(timestamp, warning=None, speed=None):
    frame = np.zeros((VIDEO_HEIGHT, VIDEO_WIDTH, 3), dtype=np.uint8)
    # Add some dummy content to simulate a dashboard and road view
    cv2.putText(frame, "Dashboard View", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.putText(frame, "Road View", (50, VIDEO_HEIGHT-50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

    # Overlay CAN data and timestamp
    cv2.putText(frame, f"Time: {timestamp}", (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    if warning:
        cv2.putText(frame, f"Warning: {warning}", (50, 150), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
    if speed is not None:
        cv2.putText(frame, f"Speed: {speed} km/h", (50, 200), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 0, 0), 2)

    return frame

def record_video(circular_buffer, stop_event):
    chunk_duration = CHUNK_DURATION_SECONDS
    chunk_frames = chunk_duration * FPS
    frame_count = 0
    chunk_frames_count = 0
    chunk_start_time = time.time()
    chunk_frames_list = []

    while not stop_event.is_set():
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        warning = None
        speed = None

        # Simulate getting CAN data (in a real scenario, this would come from the CAN bus)
        can_message = get_simulated_can_message()
        if can_message.warning:
            warning = can_message.warning
        if can_message.speed is not None:
            speed = can_message.speed

        # Generate a dummy frame
        frame = generate_dummy_frame(timestamp, warning, speed)
        chunk_frames_list.append(frame)
        frame_count += 1
        chunk_frames_count += 1

        # Check if we've reached the end of a chunk
        if chunk_frames_count >= chunk_frames:
            # Create a video chunk (in memory for simulation)
            chunk = {
                'timestamp': time.time(),
                'frames': chunk_frames_list.copy(),
                'start_time': chunk_start_time,
                'end_time': time.time()
            }
            circular_buffer.add_chunk(chunk)
            chunk_frames_list = []
            chunk_frames_count = 0
            chunk_start_time = time.time()

        # Simulate frame rate delay
        time.sleep(1/FPS)

    # Add the last partial chunk if there is one
    if chunk_frames_list:
        chunk = {
            'timestamp': time.time(),
            'frames': chunk_frames_list.copy(),
            'start_time': chunk_start_time,
            'end_time': time.time()
        }
        circular_buffer.add_chunk(chunk)

def get_simulated_can_message():
    warnings = [None, "Engine Fault", "Brake Warning", "Low Oil", "Check Engine", "ABS Fault"]
    warning = random.choice(warnings)
    speed = random.randint(0, 120) if random.random() > 0.1 else None  # 10% chance of no speed reading
    return CANMessage(warning=warning, speed=speed)

# CAN Bus Simulation
def simulate_can_bus(stop_event, trigger_event):
    while not stop_event.is_set():
        # Simulate a warning message every 30-60 seconds
        time.sleep(random.uniform(30, 60))
        warning_messages = ["Engine Fault", "Brake Warning", "Low Oil", "Check Engine", "ABS Fault"]
        warning = random.choice(warning_messages)
        print(f"CAN Warning Detected: {warning}")
        trigger_event()

# Manual Trigger Simulation
def wait_for_manual_trigger(stop_event, trigger_event):
    print("Press 't' to trigger an event manually...")
    while not stop_event.is_set():
        if select.select([sys.stdin], [], [], 0.1)[0]:
            key = sys.stdin.read(1)
            if key == 't':
                print("Manual trigger activated!")
                trigger_event()
        time.sleep(0.1)

def save_event_video(circular_buffer, event_time):
    # Determine the time window for the event
    start_time = event_time - 5 * 60  # 5 minutes before the event
    end_time = event_time + 5 * 60    # 5 minutes after the event

    # Get the relevant chunks
    relevant_chunks = []
    for chunk in circular_buffer.buffer:
        chunk_time = chunk['timestamp']
        if start_time <= chunk_time <= end_time:
            relevant_chunks.append(chunk)

    # For simplicity, we'll just print out the information about the chunks we would save
    print(f"Saving event video for time window: {datetime.datetime.fromtimestamp(start_time)} to {datetime.datetime.fromtimestamp(end_time)}")
    print(f"Found {len(relevant_chunks)} chunks in this time window.")

    # In a real scenario, you would combine these chunks into a single video file
    # and save it with a timestamp or event ID.

    # For our simulation, we'll just display the first frame of each chunk with overlays
    for i, chunk in enumerate(relevant_chunks):
        if chunk['frames']:
            # Display the first frame of the chunk
            cv2.imshow(f'Chunk {i+1} - Start: {datetime.datetime.fromtimestamp(chunk["start_time"])}',
                       chunk['frames'][0])
            cv2.waitKey(1000)  # Display each frame for 1 second
            cv2.destroyAllWindows()

def main():
    # Calculate buffer size in terms of number of chunks
    buffer_size = (BUFFER_SIZE_MINUTES * 60) // CHUNK_DURATION_SECONDS
    circular_buffer = CircularBuffer(buffer_size)

    # Create stop event for threads
    stop_event = threading.Event()

    # Create trigger event
    trigger_event = lambda: save_event_video(circular_buffer, time.time())

    # Start video recording thread
    video_thread = threading.Thread(target=record_video, args=(circular_buffer, stop_event))
    video_thread.start()

    # Start CAN bus simulation thread
    can_thread = threading.Thread(target=simulate_can_bus, args=(stop_event, trigger_event))
    can_thread.start()

    # Start manual trigger thread
    manual_trigger_thread = threading.Thread(target=wait_for_manual_trigger, args=(stop_event, trigger_event))
    manual_trigger_thread.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nStopping the simulation...")
        stop_event.set()
        video_thread.join()
        can_thread.join()
        manual_trigger_thread.join()
        cv2.destroyAllWindows()
        print("Simulation stopped.")

if __name__ == "__main__":
    main()
