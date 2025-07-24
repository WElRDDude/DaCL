# DaCL - Driver-assistant Camera Logger

## Overview

**DaCL** is a modular, multi-threaded system for automotive camera logging and CAN warning event capture. It is designed for use on a Raspberry Pi 5 with a global shutter camera and CAN bus interface, providing continuous video recording, event-based video saving, and overlaying runtime data (speed, warning type, timestamp) on all recordings. 

DaCL is ideal for prototyping and logging in driver-assistance, ADAS, or research projects where synchronized video and CAN bus event data is required.

---

## Hardware Requirements

- **Raspberry Pi 5** (Pi OS Lite 64-bit recommended)
- **Raspberry Pi Global Shutter Camera Module**
- **CAN bus interface** (Korlan USB2CAN)
- **GPIO button** (for manual event triggers)

---

## Software Dependencies

Install these packages before building:

```sh
sudo apt-get update
sudo apt-get install build-essential libopencv-dev wiringpi can-utils libcamera-dev ffmpeg
```

---

## Build Instructions

Clone the repository and build the project using the included Makefile:

```sh
make
```

---

## Running DaCL

Launch the main logger program with:

```sh
sudo ./dacl
```

Optional: add `--preview` to enable live video preview on the Pi.

---

## Features

- **Continuous video recording** in segmented `.mp4` files.
- **Disk-buffered ring** of last N minutes (default: 10) of video segments.
- **Event capture**: On trigger (via GPIO button or CAN message), saves 5 minutes before and after the event.
- **OpenCV overlays**: Speed, warning type, and timestamp are rendered on all videos.
- **Event logging**: All triggers/events logged to `logs/events.csv` with metadata.
- **Multi-threaded architecture** for video, trigger, CAN listening, and storage management.
- **Multiple CAN warnings**: Supports mapping multiple CAN IDs to human-readable warning labels.
- **Automatic cleanup**: Old video segments are deleted to maintain buffer size.
- **Configurable runtime parameters** via `configs/config.ini`.

---

## Directory Structure

- `src/` - Source code
- `configs/config.ini` - Configuration file
- `logs/events.csv` - Event log CSV file
- `/tmp/dacl_buffer/` - Default ring-buffer directory for video segments
- `/tmp/dacl_events/` - Directory for saved event videos

---

## Configuration

Edit `configs/config.ini` to set:

- `segment_seconds` - Length of each video segment (seconds)
- `buffer_minutes` - Size of rolling buffer (minutes)
- `pretrigger_minutes` / `posttrigger_minutes` - Minutes to save before/after event
- `can_iface` - CAN interface name (e.g., `can0`)
- `warning_ids` - CAN ID to label mapping, e.g. `0x123,LDW;0x456,AEB`
- `button_pin` - GPIO pin for manual trigger
- Other parameters: buffer/event directory paths, etc.

---

## File Naming Convention

Event videos are saved with the following pattern:

- `YYYYMMDD_HHMMSS_WARNINGTYPE_pretrigger.mp4`
- `YYYYMMDD_HHMMSS_WARNINGTYPE_posttrigger.mp4`

Where:
- `YYYYMMDD_HHMMSS` = timestamp of event
- `WARNINGTYPE` = label mapped from CAN ID or manual trigger
- `pretrigger` / `posttrigger` = before or after event

---

## Architecture Overview

DaCL is composed of the following key modules:

- **VideoRecorder**: Handles continuous segmented recording to buffer directory.
- **OverlayRenderer**: Uses OpenCV to generate overlay images with speed, warning, and timestamp.
- **CANListener**: Listens to the CAN bus for warning events.
- **TriggerManager**: Handles event triggers via CAN, GPIO, or console; coordinates event video saving/logging.
- **FileManager**: Copies relevant video segments to event directory and applies overlays using ffmpeg.
- **CSVLogger**: Logs all event metadata to CSV.
- **StorageManager**: Periodically deletes old segments to maintain buffer size.
- **PreviewManager**: (optional) Displays live video preview.

Inter-thread communication is via shared objects and atomic flags, ensuring reliable event capture and logging.

---

## CAN Warning Mapping

You can define multiple CAN ID mappings in `configs/config.ini`:

```
warning_ids=0x123,LDW;0x456,AEB;0x789,FCW
```

This maps CAN message IDs to warning labels (e.g., LDW = Lane Departure Warning, AEB = Autonomous Emergency Braking, FCW = Forward Collision Warning).

---

## Event Logging

All events (CAN or manual trigger) are logged in `logs/events.csv` with:

- Timestamp
- Warning type
- Trigger source
- Saved segment files
- Metadata

---

## Extending & Customization

- **Segment length/buffer size**: Change in config.
- **Additional overlays**: Modify `OverlayRenderer` to add more runtime data.
- **More CAN warnings**: Add mappings in config.
- **Alternate triggers**: Extend `TriggerManager` for new sources.

---

## Example Workflow

1. **System boots, DaCL starts.**
2. **VideoRecorder** begins segmented recording to buffer.
3. **CANListener** and **TriggerManager** watch for triggers/events.
4. On CAN warning or button press:
    - **TriggerManager** saves pre/post event segments.
    - **OverlayRenderer/FileManager** overlays metadata.
    - **CSVLogger** writes event details.
5. **StorageManager** periodically cleans buffer to maintain rolling history.

---

## License

See `LICENSE` file for details.

---

## Troubleshooting

- Insufficient disk space: Check buffer/event directories.
- CAN interface not found: Verify Korland USB2CAN setup and config.
- Camera issues: Ensure Pi camera module is detected and enabled.
- Build errors: Check dependency installation.

---
