# DaCL

A system for recording video from a lab car, with event triggering via CAN bus messages or manual input.

## Features

- Continuous video recording with rolling buffer
- Event triggering via CAN bus messages or manual button
- Saves 5 minutes before and after each event
- Overlay of warning messages, vehicle speed, and timestamp on videos

## Installation

1. Clone this repository
2. Install the required dependencies:
   ```bash
   pip install -r requirements.txt


### How to Use This Project

1. Clone this repository to your Raspberry Pi or development machine.
2. Install the required dependencies by running `pip install -r requirements.txt`.
3. Install the package in development mode with `pip install -e .`.
4. Run the system with `python -m lab_car_video_system.main` or `lab_car_video`.

For testing purposes, you can run the system on any machine with Python installed. The manual trigger simulation allows you to press 't' followed by Enter to simulate a trigger event. The system will display information about the captured event video.

For deployment on a Raspberry Pi with actual hardware:

1. Connect your camera module to the Raspberry Pi.
2. Connect your CAN bus adapter to the Raspberry Pi.
3. Connect your manual trigger button to the specified GPIO pin (default is 17).
4. Configure the CAN interface according to your setup.
5. Update the `config.py` file with your specific settings.
6. Install the required dependencies and run the system.

This complete project provides a modular and extensible framework for your lab car video recording system. Each component is separated into its own module, making it easy to modify, test, and maintain individual parts of the system. The system is designed to run continuously and handles both CAN bus triggers and manual triggers, saving relevant video footage for each event.
