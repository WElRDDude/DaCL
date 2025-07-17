# Lab Car Video System

A modular C++ system for Raspberry Pi that records vehicle video, listens to CAN bus, and saves event-based video clips. Designed for automotive labs, hobbyists, and researchers.

## Features

- Continuous video recording with circular buffer
- CAN bus warning event detection (via USB2CAN)
- Manual event trigger via GPIO button
- Pre/post event video saving (5 min before/after)
- Overlay of CAN messages, speed, timestamps
- Modular C++ code for easy extension

## Hardware

- Raspberry Pi (4 recommended)
- Global shutter camera module
- USB2CAN adapter
- Push button or switch (for GPIO)

## Quick Start

1. See [doc/INSTALL.md](doc/INSTALL.md) for setup instructions.
2. Build:  
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```
3. Run the system:
   ```sh
   ./labcar
   ```

## Directory Structure

- `src/` - source code
- `include/` - headers
- `doc/` - documentation
- `scripts/` - setup scripts

## Configuration

Adjust `config/config.ini` (if provided) or edit parameters in `main_app.cpp` for your hardware.

## License

MIT