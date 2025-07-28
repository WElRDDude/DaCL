# DaCL - Driver-assistant Camera Logger

## Overview

**DaCL** is a modular, multi-threaded system for automotive camera logging and CAN warning event capture. It is designed for use on a Raspberry Pi 5 with a global shutter camera and CAN bus interface, providing continuous video recording, event-based video saving, and overlaying runtime data (speed, warning type, timestamp) on all recordings. 

DaCL is ideal for prototyping and logging in driver-assistance, ADAS, or research projects where synchronized video and CAN bus event data is required.

---

## Quickstart Guide

### 1. Hardware Setup
- Connect Raspberry Pi Global Shutter Camera Module to RPi 5
- Connect Korlan USB2CAN interface to USB port
- Wire GPIO button to pin specified in config (default: GPIO 0)
- Ensure adequate storage space (recommend 32GB+ SD card)

### 2. Software Installation
```sh
# Update system
sudo apt-get update

# Install dependencies
sudo apt-get install build-essential libopencv-dev wiringpi can-utils libcamera-dev ffmpeg

# Clone and build DaCL
git clone https://github.com/WElRDDude/DaCL.git
cd DaCL
make

# Install development tools (optional)
make install-deps
```

### 3. Configuration
Edit `configs/config.ini` with your settings:
```ini
[Video]
segment_seconds=60
buffer_minutes=10
pretrigger_minutes=5
posttrigger_minutes=5
buffer_dir=/tmp/dacl_buffer
event_dir=/tmp/dacl_events

[CAN]
can_iface=can0
warning_ids=0x488,WarningMsg_ACM;0x481,WarningMsg_BCM

[GPIO]
button_pin=0
```

### 4. First Run
```sh
# Setup CAN interface (if using real hardware)
sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0

# Run DaCL
sudo ./dacl

# Or with preview (requires display)
sudo ./dacl --preview
```

---

## Hardware Requirements

- **Raspberry Pi 5** (Pi OS Lite 64-bit recommended)
- **Raspberry Pi Global Shutter Camera Module**
- **CAN bus interface** (Korlan USB2CAN)
- **GPIO button** (for manual event triggers)
- **32GB+ SD card** (recommend Class 10 or better)

---

## Software Dependencies

Install these packages before building:

```sh
sudo apt-get update
sudo apt-get install build-essential libopencv-dev wiringpi can-utils libcamera-dev ffmpeg
```

### Development Dependencies (Optional)
```sh
# For code formatting and documentation generation
sudo apt-get install clang-format doxygen graphviz

# Or use the Makefile target
make install-deps
```

---

## Build Instructions

Clone the repository and build the project using the included Makefile:

```sh
git clone https://github.com/WElRDDude/DaCL.git
cd DaCL
make
```

### Additional Build Targets
```sh
make clean        # Clean build artifacts
make format       # Format source code
make format-check # Check code formatting
make doc          # Generate API documentation
make doc-clean    # Clean documentation
make help         # Show all available targets
```

---

## API Documentation

Generate comprehensive API documentation using Doxygen:

```sh
make doc
```

View the generated documentation by opening `docs/html/index.html` in a web browser.

The documentation includes:
- Detailed class and method documentation
- Parameter descriptions and return values
- Usage examples and code snippets
- Thread safety information
- Error handling details

---

## Module/Class Reference

| Module | Description | Key Methods | Thread Safety |
|--------|-------------|-------------|---------------|
| **CANListener** | CAN bus interface and data parsing | `run()`, `getLatestWarning()`, `getVehicleSpeed()` | ✅ Thread-safe getters |
| **VideoRecorder** | Continuous segmented recording | `run()`, `getBufferedSegments()`, `startPostTriggerRecording()` | ✅ Mutex protected |
| **TriggerManager** | Event coordination and processing | `run()`, `handleGPIOTrigger()`, `handleCANTrigger()` | ✅ Atomic flags |
| **FileManager** | Video file operations and archival | `copyEventSegments()`, `cleanOldSegments()` | ❌ Single threaded |
| **OverlayRenderer** | OpenCV-based video annotation | `renderOverlay()` | ❌ Single threaded |
| **StorageManager** | Automatic buffer cleanup | `run()` | ✅ Background thread |
| **CSVLogger** | Event logging to CSV | `logEvent()` | ❌ Single threaded |
| **PreviewManager** | Live video preview (optional) | `run()` | ✅ Background thread |

---

## Running DaCL

Launch the main logger program with:

```sh
sudo ./dacl
```

Optional: add `--preview` to enable live video preview on the Pi.

```sh
sudo ./dacl --preview
```

---

## Sample Configuration

### Complete config.ini Example
```ini
[Video]
# Duration of each video segment in seconds
segment_seconds=60

# Total buffer duration in minutes (rolling window)
buffer_minutes=10

# Minutes to save before trigger event
pretrigger_minutes=5

# Minutes to save after trigger event
posttrigger_minutes=5

# Directory for video segment buffer
buffer_dir=/tmp/dacl_buffer

# Directory for saved event videos
event_dir=/tmp/dacl_events

[CAN]
# CAN interface name
can_iface=can0

# CAN ID to warning type mappings (hex format supported)
# Format: ID,Name;ID,Name;...
warning_ids=0x488,WarningMsg_ACM;0x481,WarningMsg_BCM;0x489,WarningMsg_CCU;0x48E,WarningMsg_DMS;0x497,WarningMsg_ECALL;0x4AA,WarningMsg_EHPS;0x4A9,WarningMsg_ESC;0x482,WarningMsg_ETGW;0x483,WarningMsg_IC;0x486,WarningMsg_PDC;0x4BB,WarningMsg_TRM;0x490,WarningMsg_TTC

[GPIO]
# GPIO pin number for manual trigger button
button_pin=0

# CAN Message ID Reference (for configuration):
# ESC_V_VEH: 0x1A1      - Vehicle Speed
# Trip_A: 0x3F4         - Trip Mileage  
# kilometerstand: 0x19D - Total Mileage
# Uhrzeit_datum: 0x2F8  - Date and Time
```

### Sample CAN Warning Mappings
```ini
# Example automotive warning messages
warning_ids=0x488,ACM_Warning;0x481,BCM_Warning;0x489,CCU_Warning;0x48E,DMS_Warning;0x497,ECALL_Warning;0x4AA,EHPS_Warning;0x4A9,ESC_Warning;0x482,ETGW_Warning;0x483,IC_Warning;0x486,PDC_Warning;0x4BB,TRM_Warning;0x490,TTC_Warning

# Common ADAS warnings
warning_ids=0x123,LDW;0x456,AEB;0x789,FCW;0x101,BSM;0x202,RCTA

# Custom application warnings  
warning_ids=0x300,CustomEvent1;0x301,CustomEvent2;0x302,TestTrigger
```

---

## Sample Output Files

### Event Log (logs/events.csv)
```csv
Timestamp,TriggerType,WarningType,Speed,PreTriggerFiles,PostTriggerFile
20240728_143022,CAN,WarningMsg_ACM,65,"video_20240728_143017.mp4,video_20240728_143018.mp4",posttrigger_20240728_143022_WarningMsg_ACM.mp4
20240728_143145,GPIO,Manual Trigger,72,"video_20240728_143140.mp4,video_20240728_143141.mp4",posttrigger_20240728_143145_Manual Trigger.mp4
20240728_143301,CONSOLE,Manual Terminal,0,"video_20240728_143256.mp4,video_20240728_143257.mp4",posttrigger_20240728_143301_Manual Terminal.mp4
```

### Video File Naming Convention
Event videos are saved with the following pattern:
- **Pre-trigger**: `YYYYMMDD_HHMMSS_WARNINGTYPE_pretrigger_N.mp4`
- **Post-trigger**: `YYYYMMDD_HHMMSS_WARNINGTYPE_posttrigger_N.mp4`

Where:
- `YYYYMMDD_HHMMSS` = timestamp of event
- `WARNINGTYPE` = label mapped from CAN ID or manual trigger
- `N` = segment number (0, 1, 2, ...)

Example files:
```
20240728_143022_WarningMsg_ACM_pretrigger_0.mp4
20240728_143022_WarningMsg_ACM_pretrigger_1.mp4
20240728_143022_WarningMsg_ACM_posttrigger_0.mp4
```

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
- **Comprehensive error handling** and input validation.
- **Thread-safe operations** with proper synchronization.

---

## Directory Structure

```
DaCL/
├── src/                    # Source code
│   ├── CANListener.*       # CAN bus interface
│   ├── VideoRecorder.*     # Video recording engine
│   ├── TriggerManager.*    # Event trigger coordination
│   ├── FileManager.*       # File operations
│   ├── OverlayRenderer.*   # Video overlay generation
│   ├── StorageManager.*    # Automatic cleanup
│   ├── CSVLogger.*         # Event logging
│   ├── PreviewManager.*    # Live preview (optional)
│   ├── utils.*             # Configuration and utilities
│   └── main.cpp            # Application entry point
├── configs/
│   └── config.ini          # Configuration file
├── logs/
│   └── events.csv          # Event log CSV file (auto-created)
├── docs/                   # Generated API documentation
├── Makefile                # Build system with dev tools
├── Doxyfile                # Doxygen configuration
└── README.md               # This file

Runtime directories (auto-created):
├── /tmp/dacl_buffer/       # Video segment buffer
└── /tmp/dacl_events/       # Saved event videos
```

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

## Architecture Overview

DaCL is composed of the following key modules:

- **VideoRecorder**: Handles continuous segmented recording to buffer directory.
- **OverlayRenderer**: Uses OpenCV to generate overlay images with speed, warning, and timestamp.
- **CANListener**: Listens to the CAN bus for warning events and vehicle data.
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

### CAN Data Sources
The system automatically extracts the following data from CAN messages:
- **Vehicle Speed** (ESC_V_VEH, ID: 0x1A1)
- **Trip Mileage** (Trip_A, ID: 0x3F4)
- **Total Mileage** (kilometerstand, ID: 0x19D)
- **Date/Time** (Uhrzeit_datum, ID: 0x2F8)

---

## Event Logging

All events (CAN or manual trigger) are logged in `logs/events.csv` with:

- Timestamp
- Warning type
- Trigger source
- Vehicle speed at time of event
- Saved segment files
- Metadata

---

## Extending & Customization

- **Segment length/buffer size**: Change in config.
- **Additional overlays**: Modify `OverlayRenderer` to add more runtime data.
- **More CAN warnings**: Add mappings in config.
- **Alternate triggers**: Extend `TriggerManager` for new sources.
- **Custom video processing**: Modify `FileManager` overlay commands.

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

### Common Issues and Solutions

#### Build Errors
**Problem**: Compilation fails with missing headers
```
fatal error: opencv2/opencv.hpp: No such file or directory
```
**Solution**: Install OpenCV development packages
```sh
sudo apt-get install libopencv-dev
```

**Problem**: Missing wiringPi.h
```
fatal error: wiringPi.h: No such file or directory
```
**Solution**: Install wiringPi library
```sh
sudo apt-get install libwiringpi-dev
```

#### Runtime Errors

**Problem**: Insufficient disk space for video buffer
```
Error: Failed to create event directory: No space left on device
```
**Solution**: 
- Check available disk space: `df -h`
- Clean old event videos: `rm -rf /tmp/dacl_events/*`
- Reduce buffer size in config: `buffer_minutes=5`

**Problem**: CAN interface not found
```
Error: ioctl SIOCGIFINDEX: No such device
```
**Solution**: 
- Check CAN interface exists: `ip link show`
- Setup CAN interface: `sudo ip link set can0 type can bitrate 500000 && sudo ip link set up can0`
- Verify config.ini has correct interface name

**Problem**: Camera initialization fails
```
Error: Failed to open camera device
```
**Solution**:
- Enable camera: `sudo raspi-config` → Interface Options → Camera
- Check camera connection and ribbon cable
- Verify camera detected: `libcamera-hello --list-cameras`

**Problem**: Permission denied errors
```
Error: Permission denied accessing GPIO
```
**Solution**: Run with sudo privileges
```sh
sudo ./dacl
```

#### Configuration Issues

**Problem**: No CAN warnings being detected
**Solution**:
- Verify CAN bus has traffic: `candump can0`
- Check warning_ids mapping in config.ini
- Ensure CAN bitrate matches network: typically 250k or 500k

**Problem**: GPIO button not triggering events
**Solution**:
- Check button wiring and GPIO pin number in config
- Test GPIO pin: `gpio read <pin_number>`
- Verify pull-up/pull-down resistor configuration

**Problem**: Video overlay not appearing
**Solution**:
- Check ffmpeg installation: `which ffmpeg`
- Verify overlay image generation in /tmp/dacl_overlay_*.png
- Check system resources and CPU usage

#### Performance Issues

**Problem**: High CPU usage causing frame drops
**Solution**:
- Reduce video resolution in libcamera-vid command
- Increase segment_seconds to reduce file I/O
- Disable preview mode if not needed

**Problem**: Storage filling up quickly
**Solution**:
- Reduce buffer_minutes in configuration
- Reduce pretrigger_minutes and posttrigger_minutes
- Setup automatic cleanup with shorter intervals

#### Development Issues

**Problem**: Code formatting inconsistent
**Solution**: Use automated formatting
```sh
make format
```

**Problem**: Documentation not generating
**Solution**: Install doxygen and graphviz
```sh
make install-deps
make doc
```

### Debug Mode

For enhanced debugging, modify the source to increase logging verbosity or use a debugger:

```sh
# Compile with debug symbols
make clean
CXXFLAGS="-g -O0" make

# Run with gdb
sudo gdb ./dacl
(gdb) run
```

### Log Analysis

Monitor system logs for additional error information:
```sh
# System logs
sudo journalctl -f

# DaCL-specific errors in syslog
sudo tail -f /var/log/syslog | grep dacl
```

### Support

If issues persist:
1. Check the [project issues](https://github.com/WElRDDude/DaCL/issues) on GitHub
2. Verify all dependencies are correctly installed
3. Test with minimal configuration
4. Provide system information, config.ini, and error logs when reporting issues

---
