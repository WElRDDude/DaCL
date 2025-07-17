### Installation and Usage Guide

#### Hardware Requirements
1. Raspberry Pi (any model with camera support)
2. Raspberry Pi Camera Module (global shutter recommended)
3. CAN bus adapter (USB2CAN or similar)
4. Manual trigger button or switch
5. MicroSD card (32GB or larger recommended)
6. Power supply for Raspberry Pi

#### Software Requirements
1. Raspberry Pi OS (preferably the latest version)
2. C++ compiler (e.g., g++)
3. Required libraries:
   - libcam
   - libcan
   - OpenCV
   - WiringPi

#### Installation Steps

1. **Set up Raspberry Pi OS**:
   - Download and install Raspberry Pi OS on a microSD card using the Raspberry Pi Imager.
   - Insert the microSD card into the Raspberry Pi and boot it up.
   - Complete the initial setup (language, Wi-Fi, etc.).

2. **Enable Camera and Other Interfaces**:
   - Run `sudo raspi-config`.
   - Navigate to Interface Options and enable the Camera, SPI, and I2C interfaces.
   - Finish and reboot.

3. **Install Dependencies**:
   - Update your system and install required packages:
     ```bash
     sudo apt update
     sudo apt upgrade -y
     sudo apt install -y g++ libopencv-dev libcam-dev wiringpi
     ```

4. **Set up CAN Interface**:
   - If using a CAN bus adapter, load the necessary kernel modules and bring up the CAN interface:
     ```bash
     sudo modprobe can
     sudo modprobe can_raw
     sudo modprobe can_dev
     sudo modprobe gs_usb
     sudo ip link set can0 up type can bitrate 500000
     ```

5. **Connect Hardware**:
   - Connect the camera module to the Raspberry Pi's camera port.
   - Connect the CAN bus adapter to a USB port on the Raspberry Pi.
   - Connect the manual trigger button to a GPIO pin (e.g., GPIO 18) and ground.

6. **Build the Project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

7. **Run the System**:
   ```bash
   ./lab_car_video_system
   ```
