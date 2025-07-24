# DaCL - Driver-assistant Camera Logger

## Hardware
- Raspberry Pi 5 (Pi OS Lite 64-bit recommended)
- Raspberry Pi Global Shutter Camera Module
- CAN bus interface (Pi CAN Hat or similar)
- GPIO button

## Software Dependencies
```sh
sudo apt-get update
sudo apt-get install build-essential libopencv-dev wiringpi can-utils libcamera-dev ffmpeg
```

## Build
```sh
make
```

## Run
```sh
sudo ./dacl
```

## Features
- Continuous video recording in segmented .mp4 files
- Disk-buffered ring of last 10 minutes of video segments
- On trigger (GPIO, CAN message), saves 5 minutes before and after event
- Overlays (speed, warning type, timestamp) rendered on all videos using OpenCV
- Logs all events to `logs/events.csv` with detailed metadata
- Fully modular multi-threaded architecture
- Supports multiple CAN warnings mapped to human-readable labels
- Automatic cleanup of old video segments to maintain buffer size
- Configurable runtime parameters via `configs/config.ini`

## File Naming
- `YYYYMMDD_HHMMSS_WARNINGTYPE_pretrigger.mp4`
- `YYYYMMDD_HHMMSS_WARNINGTYPE_posttrigger.mp4`

## Configuration
- `configs/config.ini` for runtime parameters (segment length, buffer size, CAN warning mapping, etc.)

## Multiple CAN Warnings
- The system supports multiple CAN IDs, each mapped to a human-readable warning label.