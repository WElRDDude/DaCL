# Setup & Installation Guide: Lab Car Video System

## Hardware Requirements

- Raspberry Pi 4 (or compatible)
- Raspberry Pi OS (64-bit recommended)
- Global Shutter Camera Module (CSI or USB)
- USB2CAN Adapter (e.g., Seeed, ValueCAN, etc.)
- Push button or switch for manual trigger (connected to GPIO)
- Sufficient SD card (32GB+ recommended)

## Software Dependencies

- C++17 compiler (g++ >= 9)
- CMake >= 3.12
- libcamera (`sudo apt install libcamera-apps`)
- ffmpeg (`sudo apt install ffmpeg`)
- CAN utilities (`sudo apt install can-utils`)
- (Optional) WiringPi or pigpio for real GPIO handling

## Setup Steps

### 1. Prepare the Hardware

- Attach the global shutter camera to the CSI port.
- Plug in USB2CAN adapter to a USB port.
- Connect the button/switch to a GPIO pin (e.g., GPIO17).

### 2. Enable Camera and CAN Bus

- Enable camera in Raspberry Pi config:  
  `sudo raspi-config` → Interface Options → Camera → Enable
- Setup CAN:  
  Add to `/boot/config.txt` if using PiCAN board, or load USB2CAN drivers if USB.

### 3. Install Required Packages

```sh
sudo apt update
sudo apt install cmake g++ libcamera-apps ffmpeg can-utils
```

### 4. Download and Build Project

Clone the repository:

```sh
git clone https://github.com/WElRDDude/DaCL.git
cd DaCL
mkdir build && cd build
cmake ..
make
```

### 5. Configure

- Edit `main_app.cpp` or create `config/config.ini` to match your hardware (camera, CAN device, GPIO pin).
- Adjust buffer/event sizes as needed.

### 6. Run

```sh
cd build
./labcar
```

- Video chunks will be saved in `./buffer/`.
- Event clips will be saved in `./events/`.

### 7. Stopping and Cleaning Up

- Press `Ctrl+C` to stop.
- Clean old video chunks by running:
  ```sh
  rm -rf buffer/*
  rm -rf events/*
  ```

## Notes

- GPIO and CAN code is simulated for demonstration. Replace with actual device access code as needed for production.
- For advanced overlay, consider customizing ffmpeg or using libcamera overlays.
- For fleet or research use, integrate with upload/backup scripts as needed.

## Troubleshooting

- If the camera doesn't work, check with `libcamera-hello`.
- For CAN bus issues, check with `candump can0`.

## Extending

- Add more overlays (GPS, IMU, etc.)
- Use a web UI for live event review.
- Deploy to multiple vehicles for fleet management.

---